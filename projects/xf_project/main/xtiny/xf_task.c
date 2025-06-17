/**
 * @file xf_task.c
 * @author catcatBlue (catcatblue@qq.com)
 * @brief
 * @version 1.0
 * @date 2025-06-16
 *
 * SPDX-FileCopyrightText: 2025 CompanyNameMagicTag
 * SPDX-License-Identifier: Apache-2.0
 *
 */

/* ==================== [Includes] ========================================== */

#include "xf_task.h"

/* ==================== [Defines] =========================================== */

#define TAG "xf_task"

/* ==================== [Typedefs] ========================================== */

/* 内部循环变量均使用 uint8_t */
STATIC_ASSERT(XF_TASK_NUM_MAX < ((uint8_t)~(uint8_t)0));

/* ==================== [Static Prototypes] ================================= */

static xf_err_t xf_task_sched(void *arg);

static void xf_task_sched_resume(void);
static void xf_task_sched_suspend(void);

/* ==================== [Static Variables] ================================== */

/* 协程池 */
static xf_task_t s_task_pool[XF_TASK_NUM_MAX] = {0};

/* 嵌套深度 */
static volatile int8_t s_nest_depth = 0;

/* 调度器定时任务 */
static xf_stimer_t *s_sched_stimer = NULL;

/* ==================== [Macros] ============================================ */

/* ==================== [Global Functions] ================================== */

xf_task_t *xf_task_acquire(void)
{
    uint8_t i;
    for (i = 0; i < XF_TASK_NUM_MAX; ++i) {
        if (s_task_pool[i].func == NULL) {
            return &s_task_pool[i];
        }
    }
    return NULL;
}

xf_err_t xf_task_release(xf_task_t *task)
{
    if (task == NULL) {
        return XF_ERR_INVALID_ARG;
    }
    task->func = NULL;
    return XF_OK;
}

xf_err_t xf_task_init(xf_task_t *task, xf_task_func_t func, void *user_data)
{
    if ((task == NULL) || (func == NULL)) {
        return XF_ERR_INVALID_ARG;
    }
    xf_task_lc_init(xf_task_cast(task)->lc);
    xf_task_cast(task)->func = func;
    xf_task_cast(task)->user_data = user_data;
    task->id_stimer = XF_STIMER_ID_INVALID;
    task->id_subscr = XF_PS_ID_INVALID;
    task->id_parent = XF_TASK_ID_INVALID;
    task->id_child = XF_TASK_ID_INVALID;
    xf_task_attr_set_state(task, XF_TASK_READY);
    return XF_OK;
}

xf_err_t xf_task_deinit(xf_task_t *task)
{
    if (task == NULL) {
        return XF_ERR_INVALID_ARG;
    }
    xf_memset(task, 0, sizeof(xf_task_t));
    return XF_OK;
}

xf_task_t *xf_task_create_(xf_task_t *parent, xf_task_func_t func, void *user_data)
{
    xf_task_t *task = xf_task_acquire();
    if (task == NULL) {
        XF_FATAL_ERROR();
        return NULL;
    }
    xf_task_init(task, func, user_data);
    if (parent != NULL) {
        task->id_parent = xf_task_to_id(parent);
        parent->id_child = xf_task_to_id(task);
        /* 子任务不需要 xf_task_sched 来调度 */
    } else {
        /* 恢复 s_sched_stimer ，调度所有顶级任务 */
        xf_task_sched_resume();
    }
    return task;
}

xf_err_t xf_task_destroy(xf_task_t *task)
{
    if (task == NULL) {
        return XF_ERR_INVALID_ARG;
    }
    xf_task_deinit(task);
    xf_task_release(task);
    return XF_OK;
}

xf_task_id_t xf_task_to_id(xf_task_t *s)
{
    intptr_t idx;
    if ((s == NULL)
            || (s < &s_task_pool[0])
            || (s > &s_task_pool[XF_TASK_NUM_MAX - 1])) {
        return XF_STIMER_ID_INVALID;
    }
    idx = ((intptr_t)s - (intptr_t)&s_task_pool[0]) / (sizeof(s_task_pool[0]));
    return (xf_task_id_t)idx;
}

xf_task_t *xf_task_id_to_task(xf_task_id_t id)
{
    if (id >= XF_TASK_NUM_MAX) {
        return NULL;
    }
    return &s_task_pool[id];
}

xf_err_t xf_task_sched_init(void)
{
    if (s_sched_stimer == NULL) {
        s_sched_stimer = xf_stimer_create(
                             XF_STIMER_INFINITY,
                             (xf_stimer_cb_t)xf_task_sched_timer_cb,
                             NULL);
        if (s_sched_stimer == NULL) {
            XF_FATAL_ERROR();
        }
    }
    return XF_OK;
}

xf_err_t xf_task_sched_deinit(void)
{
    if (s_sched_stimer != NULL) {
        xf_stimer_destroy(s_sched_stimer);
        s_sched_stimer = NULL;
    }
    return XF_OK;
}

xf_err_t xf_task_acquire_timer(xf_task_t *me, xf_tick_t tick_period)
{
    xf_stimer_t *stimer;
    if (me == NULL) {
        return XF_ERR_INVALID_ARG;
    }
    if (me->id_stimer != XF_STIMER_ID_INVALID) {
        return XF_ERR_INITED;
    }
    stimer = xf_stimer_create(
                 tick_period,
                 (xf_stimer_cb_t)xf_resume_task_timer_cb,
                 (void *)(me));
    if (stimer == NULL) {
        XF_FATAL_ERROR();
    }
    me->id_stimer = xf_stimer_to_id(stimer);
    return XF_OK;
}

xf_err_t xf_task_release_timer(xf_task_t *me)
{
    xf_stimer_t *stimer;
    if (me == NULL) {
        return XF_ERR_INVALID_ARG;
    }
    if (me->id_stimer != XF_STIMER_ID_INVALID) {
        stimer = xf_stimer_id_to_stimer(me->id_stimer);
        if (stimer == NULL) {
            XF_FATAL_ERROR();
        }
        xf_stimer_destroy(stimer);
        me->id_stimer = XF_STIMER_ID_INVALID;
    }
    return XF_OK;
}

void xf_resume_task_timer_cb(xf_stimer_t *stimer)
{
    xf_task_t *parent;
    xf_task_t *task = xf_task_cast(stimer->user_data);
    if (task->id_stimer != xf_stimer_to_id(stimer)) {
        XF_FATAL_ERROR();
    }

    /* 将自己和所有父级都设为 ready */
    xf_task_set_ready(task);
    parent = xf_task_id_to_task(task->id_parent);
    while (parent != NULL) {
        xf_task_set_ready(parent);
        parent = xf_task_id_to_task(parent->id_parent);
    }

    task->id_stimer = XF_STIMER_ID_INVALID;
    xf_stimer_destroy(stimer);
    /* 恢复 s_sched_stimer */
    xf_task_sched_resume();
}

void xf_task_sched_timer_cb(xf_stimer_t *stimer)
{
    UNUSED(stimer);
    xf_task_sched(NULL);
    xf_task_sched_suspend();
}

void xf_task_nest_depth_inc(void)
{
    ++s_nest_depth;
    if (xf_task_get_nest_depth() >= XF_TASK_NEST_DEPTH_MAX) {
        XF_FATAL_ERROR();
    }
}

void xf_task_nest_depth_dec(void)
{
    --s_nest_depth;
}

int8_t xf_task_get_nest_depth(void)
{
    return s_nest_depth;
}

#if XF_TODO

void xf_subscr_call_co_cb(xf_ps_subscr_t *s, xf_event_t *e)
{
    xf_task_state_t co_state;
    xf_task_t *task = xf_task_cast(s->user_data);
    if (task->s != s) {
        XF_FATAL_ERROR();
    }
    xf_task_resume(task, task, e, co_state);
    UNUSED(co_state);
}

xf_err_t xf_task_attach_stimer(xf_task_t *task, xf_stimer_t *stimer)
{
    if ((task == NULL) || (stimer == NULL)) {
        return XF_ERR_INVALID_ARG;
    }
    if (task->t != NULL) {
        return XF_ERR_INITED;
    }
    task->t = stimer;
    stimer->user_data = task;
    return XF_OK;
}

xf_err_t xf_task_detach_stimer(xf_task_t *task, xf_stimer_t *stimer)
{
    if ((task == NULL) || (stimer == NULL)) {
        return XF_ERR_INVALID_ARG;
    }
    if (task->t != stimer) {
        return XF_ERR_INVALID_ARG;
    }
    task->t = NULL;
    stimer->user_data = NULL;
    return XF_OK;
}

xf_err_t xf_task_attach_subscriber(xf_task_t *task, xf_ps_subscr_t *s)
{
    if ((task == NULL) || (s == NULL)) {
        return XF_ERR_INVALID_ARG;
    }
    if (task->s != NULL) {
        return XF_ERR_INITED;
    }
    task->s = s;
    s->user_data = task;
    return XF_OK;
}

xf_err_t xf_task_detach_subscriber(xf_task_t *task, xf_ps_subscr_t *s)
{
    if ((task == NULL) || (s == NULL)) {
        return XF_ERR_INVALID_ARG;
    }
    if (task->s != s) {
        return XF_ERR_INVALID_ARG;
    }
    task->s = NULL;
    s->user_data = NULL;
    return XF_OK;
}

xf_err_t xf_task_teardown_wait_until(xf_task_t *me)
{
    if (me == NULL) {
        return XF_ERR_INVALID_ARG;
    }
    if (me->t) {
        xf_stimer_destroy(me->t);
        me->t = NULL;
    }
    if (me->s) {
        xf_ps_unsubscribe_all(me->s);
        xf_ps_destroy_subscriber(me->s);
        me->s = NULL;
    }
    return XF_OK;
}

xf_err_t xf_task_setup_wait_until_1(
    xf_task_t *me, xf_event_id_t id_1, xf_tick_t tick)
{
    xf_err_t xf_ret;
    if (tick != XF_STIMER_INFINITY) {
        me->t = xf_stimer_create_oneshot(
                    (tick),
                    (xf_stimer_cb_t)xf_stimer_call_co_cb,
                    (void *)(me));
        if (me->t == NULL) {
            XF_FATAL_ERROR();
        }
    }
    me->s = xf_ps_create_subscriber(
                (xf_ps_subscr_cb_t)xf_subscr_call_co_cb,
                (void *)(me));
    if (!me->s) {
        XF_FATAL_ERROR();
    }
    xf_ret = xf_ps_subscribe(me->s, id_1);
    if (xf_ret != XF_OK) {
        goto l_err;
    }
    return xf_ret;
l_err:;
    xf_task_teardown_wait_until(me);
    return xf_ret;
}

xf_err_t xf_task_setup_wait_until_2(
    xf_task_t *me, xf_event_id_t id_1, xf_event_id_t id_2, xf_tick_t tick)
{
    xf_err_t xf_ret;
    xf_ret = xf_task_setup_wait_until_1(me, id_1, tick);
    if (xf_ret != XF_OK) {
        return xf_ret;
    }
    xf_ret = xf_ps_subscribe(me->s, id_2);
    if (xf_ret != XF_OK) {
        goto l_err;
    }
    return xf_ret;
l_err:;
    xf_task_teardown_wait_until(me);
    return xf_ret;
}

#endif

/* ==================== [Static Functions] ================================== */

static void xf_task_sched_resume(void)
{
    xf_stimer_set_period(s_sched_stimer, 0);
    xf_stimer_set_ready(s_sched_stimer);
}

static void xf_task_sched_suspend(void)
{
    xf_stimer_set_period(s_sched_stimer, XF_STIMER_INFINITY);
}

static xf_err_t xf_task_sched(void *arg)
{
    uint8_t i;
    xf_task_t dummy;
    xf_task_t *me = &dummy;
    xf_task_state_t task_state;
    xf_task_t *task;
    UNUSED(task_state);
    for (i = 0; i < XF_TASK_NUM_MAX; ++i) {
        task = &s_task_pool[i];
        if ((task->func)
                && (xf_task_attr_get_state(task) == XF_TASK_READY)
                && (task->id_parent == XF_TASK_ID_INVALID) /*!< 只调度顶级任务 */
           ) {
            xf_task_resume(me, task, arg, task_state);
        }
    }
    return XF_OK;
}

#if XF_TODO

static xf_task_state_t xf_task_top(xf_task_top_t *me, void *arg)
{
    xf_task_begin(me);
    me->co_main = xf_task_create(xf_task_main, 0);
    me->co_main_state = XF_TASK_BLOCKED;
    while (1) {
        if (me->co_main_state != XF_TASK_TERMINATED) {
            xf_task_resume(me, me->co_main, arg, me->co_main_state);
        }
        xf_task_yield(me);
    }
    xf_task_end(me);
}

#endif
