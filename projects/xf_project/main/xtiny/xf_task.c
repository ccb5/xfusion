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
static xf_err_t xf_task_resume_root(xf_task_t *task, void *arg);

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
        if (s_task_pool[i].cb_func == NULL) {
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
    task->cb_func = NULL;
    return XF_OK;
}

xf_err_t xf_task_init(xf_task_t *task, xf_task_cb_t cb_func, void *user_data)
{
    if ((task == NULL) || (cb_func == NULL)) {
        return XF_ERR_INVALID_ARG;
    }
    xf_task_lc_init(xf_task_cast(task)->lc);
    xf_task_cast(task)->cb_func = cb_func;
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

xf_task_t *xf_task_create_(xf_task_t *parent, xf_task_cb_t cb_func, void *user_data)
{
    xf_task_t *task = xf_task_acquire();
    if (task == NULL) {
        XF_FATAL_ERROR();
        return NULL;
    }
    xf_task_init(task, cb_func, user_data);
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

xf_err_t xf_task_destroy_(xf_task_t *task)
{
    if (task == NULL) {
        return XF_ERR_INVALID_ARG;
    }
    xf_task_deinit(task);
    xf_task_release(task);
    return XF_OK;
}

xf_task_id_t xf_task_to_id(const xf_task_t *task)
{
    if ((task == NULL)
            || (task < &s_task_pool[0])
            || (task > &s_task_pool[XF_TASK_NUM_MAX - 1])) {
        return XF_STIMER_ID_INVALID;
    }
    return (xf_task_id_t)(task - &s_task_pool[0]);
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

xf_err_t xf_task_acquire_subscr(xf_task_t *me, xf_event_id_t id)
{
    xf_ps_subscr_t *s;
    if (me == NULL) {
        return XF_ERR_INVALID_ARG;
    }
    if (me->id_subscr != XF_PS_ID_INVALID) {
        return XF_ERR_INITED;
    }
    s = xf_subscribe(id, xf_resume_task_subscr_cb, me);
    if (s == NULL) {
        XF_FATAL_ERROR();
    }
    me->id_subscr = xf_ps_subscr_to_id(s);
    return XF_OK;
}

xf_err_t xf_task_release_subscr(xf_task_t *me)
{
    if (me == NULL) {
        return XF_ERR_INVALID_ARG;
    }
    if (me->id_subscr != XF_PS_ID_INVALID) {
        xf_unsubscribe_by_id(me->id_subscr);
        me->id_subscr = XF_PS_ID_INVALID;
    }
    return XF_OK;
}

void xf_resume_task_timer_cb(xf_stimer_t *stimer)
{
    xf_task_t *task = xf_task_cast(stimer->user_data);
    if (task->id_stimer != xf_stimer_to_id(stimer)) {
        XF_FATAL_ERROR();
    }
    xf_task_release_timer(task);
    xf_task_resume_root(task, NULL);
}

void xf_resume_task_subscr_cb(xf_subscr_t *s, uint8_t ref_cnt, void *arg)
{
    UNUSED(ref_cnt);
    UNUSED(arg);
    xf_task_t *task = xf_task_cast(s->user_data);
    if (task->id_subscr != xf_ps_subscr_to_id(s)) {
        XF_FATAL_ERROR();
    }
    xf_task_release_subscr(task);
    xf_task_resume_root(task, arg);
}

xf_err_t xf_task_setup_wait_until(
    xf_task_t *me, xf_event_id_t id, xf_tick_t tick_period)
{
    if (id == XF_EVENT_ID_INVALID) {
        return XF_ERR_INVALID_ARG;
    }
    if (tick_period != XF_STIMER_INFINITY) {
        xf_task_acquire_timer(me, tick_period);
    }
    xf_task_acquire_subscr(me, id);
    return XF_OK;
}

xf_err_t xf_task_teardown_wait_until(xf_task_t *me)
{
    if (me == NULL) {
        return XF_ERR_INVALID_ARG;
    }
    xf_task_release_timer(me);
    xf_task_release_subscr(me);
    return XF_OK;
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

static xf_err_t xf_task_resume_root(xf_task_t *task, void *arg)
{
    xf_task_t *parent;
    xf_task_t *root;
    if (task == NULL) {
        return XF_ERR_INVALID_ARG;
    }
    /* 将自己和所有父级都设为 ready, 并调用 root */
    parent = xf_task_id_to_task(task->id_parent);
    if (parent == NULL) {
        root = task;
    } else {
        do {
            xf_task_set_ready(parent);
            root = parent;
            parent = xf_task_id_to_task(parent->id_parent);
        } while (parent != NULL);
    }
    xf_task_run(root, arg);
    return XF_OK;
}

static xf_err_t xf_task_sched(void *arg)
{
    uint8_t i;
    xf_task_t *task;
    for (i = 0; i < XF_TASK_NUM_MAX; ++i) {
        task = &s_task_pool[i];
        if ((task->cb_func)
                && (xf_task_attr_get_state(task) == XF_TASK_READY)
                && (task->id_parent == XF_TASK_ID_INVALID) /*!< 只调度顶级任务 */
           ) {
            xf_task_run_direct(task, arg);
        }
    }
    return XF_OK;
}
