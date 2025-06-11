/**
 * @file xf_co.c
 * @author catcatBlue (catcatblue@qq.com)
 * @brief
 * @version 1.0
 * @date 2025-05-19
 *
 * SPDX-FileCopyrightText: 2025 CompanyNameMagicTag
 * SPDX-License-Identifier: Apache-2.0
 *
 */

/* ==================== [Includes] ========================================== */

#include "xf_co.h"
#include "xf_ps.h"

/* ==================== [Defines] =========================================== */

#define TAG "xf_co"

/* ==================== [Typedefs] ========================================== */

/* ==================== [Static Prototypes] ================================= */

static xf_co_state_t xf_co_top(xf_co_top_t *const me, void *arg);

/* ==================== [Static Variables] ================================== */

/* 协程池 */
static xf_co_t xf_co_pool[XF_CO_NUM_MAX] = {0};

static volatile int8_t s_nest_depth = 0;

xf_co_top_t s_co_top_ctx = {0};
xf_co_top_t *const sp_top = &s_co_top_ctx;

/* ==================== [Macros] ============================================ */

/* ==================== [Global Functions] ================================== */

xf_co_top_t *xf_co_get_top(void)
{
    return sp_top;
}

void xf_co_nest_depth_inc(void)
{
    ++s_nest_depth;
}

void xf_co_nest_depth_dec(void)
{
    --s_nest_depth;
}

int8_t xf_co_get_nest_depth(void)
{
    return s_nest_depth;
}

void xf_stimer_call_co_cb(xf_stimer_t *const stimer)
{
    xf_co_state_t co_state;
    xf_co_t *co = xf_co_cast(stimer->user_data);
    if (co->t != stimer) {
        XF_FATAL_ERROR();
    }
    xf_co_resume(co, co, stimer, co_state);
    UNUSED(co_state);
}

void xf_subscr_call_co_cb(xf_ps_subscr_t *const s, xf_event_t *e)
{
    xf_co_state_t co_state;
    xf_co_t *co = xf_co_cast(s->user_data);
    if (co->s != s) {
        XF_FATAL_ERROR();
    }
    xf_co_resume(co, co, e, co_state);
    UNUSED(co_state);
}

xf_err_t xf_co_attach_stimer(xf_co_t *const co, xf_stimer_t *const stimer)
{
    if ((co == NULL) || (stimer == NULL)) {
        return XF_ERR_INVALID_ARG;
    }
    if (co->t != NULL) {
        return XF_ERR_INITED;
    }
    co->t = stimer;
    stimer->user_data = co;
    return XF_OK;
}

xf_err_t xf_co_detach_stimer(xf_co_t *const co, xf_stimer_t *const stimer)
{
    if ((co == NULL) || (stimer == NULL)) {
        return XF_ERR_INVALID_ARG;
    }
    if (co->t != stimer) {
        return XF_ERR_INVALID_ARG;
    }
    co->t = NULL;
    stimer->user_data = NULL;
    return XF_OK;
}

xf_err_t xf_co_attach_subscriber(xf_co_t *const co, xf_ps_subscr_t *const s)
{
    if ((co == NULL) || (s == NULL)) {
        return XF_ERR_INVALID_ARG;
    }
    if (co->s != NULL) {
        return XF_ERR_INITED;
    }
    co->s = s;
    s->user_data = co;
    return XF_OK;
}

xf_err_t xf_co_detach_subscriber(xf_co_t *const co, xf_ps_subscr_t *const s)
{
    if ((co == NULL) || (s == NULL)) {
        return XF_ERR_INVALID_ARG;
    }
    if (co->s != s) {
        return XF_ERR_INVALID_ARG;
    }
    co->s = NULL;
    s->user_data = NULL;
    return XF_OK;
}

xf_err_t xf_co_teardown_wait_until(xf_co_t *const me)
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

xf_err_t xf_co_setup_wait_until_1(
    xf_co_t *const me, xf_event_id_t id_1, xf_tick_t tick)
{
    xf_err_t xf_ret;
    me->t = xf_stimer_create_oneshot(
                (tick),
                (xf_stimer_cb_t)xf_stimer_call_co_cb,
                (void *)(me));
    me->s = xf_ps_create_subscriber(
                (xf_ps_subscr_cb_t)xf_subscr_call_co_cb,
                (void *)(me));
    if ((!me->t) || (!me->s)) {
        XF_FATAL_ERROR();
    }
    xf_ret = xf_ps_subscribe(me->s, id_1);
    if (xf_ret != XF_OK) {
        goto l_err;
    }
    return xf_ret;
l_err:;
    xf_co_teardown_wait_until(me);
    return xf_ret;
}

xf_err_t xf_co_setup_wait_until_2(
    xf_co_t *const me, xf_event_id_t id_1, xf_event_id_t id_2, xf_tick_t tick)
{
    xf_err_t xf_ret;
    xf_ret = xf_co_setup_wait_until_1(me, id_1, tick);
    if (xf_ret != XF_OK) {
        return xf_ret;
    }
    xf_ret = xf_ps_subscribe(me->s, id_2);
    if (xf_ret != XF_OK) {
        goto l_err;
    }
    return xf_ret;
l_err:;
    xf_co_teardown_wait_until(me);
    return xf_ret;
}

xf_err_t xf_co_top_init(void)
{
    xf_co_cfg_t co_cfg = {0};
    if (sp_top->base.func == NULL) {
        co_cfg.id = (XF_CO_NUM_MAX - 1);
        co_cfg.type = XF_CO_TYPE_TOP;
        xf_co_ctor(xf_co_cast(&s_co_top_ctx), xf_co_func_cast(xf_co_top), NULL, &co_cfg);
        return XF_OK;
    }
    return XF_ERR_INITED;
}

xf_err_t xf_co_top_run(void *arg)
{
    xf_err_t xf_ret = XF_OK;
    xf_co_state_t res;
    xf_co_t *me = xf_co_cast(&s_co_top_ctx);
    xf_co_resume(me, me, arg, res);
    UNUSED(res);
    return xf_ret;
}

xf_err_t xf_co_ctor(
    xf_co_t *const co, xf_co_func_t func, void *user_data, xf_co_cfg_t *p_cfg)
{
    if ((!co) || (!func)) {
        return XF_ERR_INVALID_ARG;
    }
    if (p_cfg) {
        /* 检查此 id 是否已被占用 */
        if (NULL != xf_co_pool[p_cfg->id].func) {
            XF_ERROR_LINE();
            return XF_FAIL;
        }
        xf_co_attr_set_type(co, p_cfg->type);
        xf_co_attr_set_id(co, p_cfg->id);
    } else {
        xf_co_attr_set_type(co, XF_CO_TYPE_UNKNOWN);
    }
    xf_co_lc_init(xf_co_cast(co)->lc);
    xf_co_cast(co)->func = func;
#if CONFIG_XF_CO_ENABLE_USER_DATA
    xf_co_cast(co)->user_data = user_data;
#else
    UNUSED(user_data);
#endif
    xf_co_attr_set_state(co, XF_CO_SUSPENDED);
    xf_co_attr_set_await_bit(co, FALSE);
    /* 不改变 xf_co_attr_set_reserved */
    return XF_OK;
}

xf_err_t xf_co_dtor(xf_co_t *const co)
{
    if ((!co)) {
        return XF_ERR_INVALID_ARG;
    }
    const xf_co_t xf_co_empty = {0};
    *xf_co_cast(co) = xf_co_empty;
    return XF_OK;
}

xf_co_t *xf_co_create_(
    xf_co_func_t func, void *user_data, xf_co_cfg_t *p_cfg)
{
    int16_t i = 0;
    xf_err_t xf_ret;
    xf_co_cfg_t cfg = {0};
    if (!func) {
        return NULL;
    }
    /* 此处决定优先分配小的协程 id */
    for (i = 0; i < (int16_t)XF_CO_NUM_MAX; ++i) {
        if (NULL == xf_co_pool[i].func) {
            break;
        }
    }
    if (i == XF_CO_NUM_MAX) {
        XF_FATAL_ERROR();
        return NULL;
    }
    if (NULL == p_cfg) {
        cfg.type = XF_CO_TYPE_UNKNOWN;
        cfg.id = i;
        p_cfg = &cfg;
    }
    xf_ret = xf_co_ctor(&xf_co_pool[i], func, user_data, p_cfg);
    if (xf_ret != XF_OK) {
        XF_FATAL_ERROR();
        return NULL;
    }
    return &xf_co_pool[i];
}

xf_err_t xf_co_destroy(xf_co_t *co)
{
    int16_t i = 0;
    xf_err_t xf_ret;
    xf_co_id_t id;
    if (!co) {
        return XF_ERR_INVALID_ARG;
    }
    id = xf_co_attr_get_id(co);
    if (id > (XF_CO_NUM_MAX - 1)) {
        XF_ERROR_LINE();
        return XF_FAIL;
    }
    if (co == &xf_co_pool[i]) {
        xf_ret = xf_co_dtor(co);
        return xf_ret;
    }
    return XF_FAIL;
}

/* ==================== [Static Functions] ================================== */

static xf_co_state_t xf_co_top(xf_co_top_t *const me, void *arg)
{
    xf_co_begin(me);
    me->co_main = xf_co_create(xf_co_main, 0);
    me->co_main_state = XF_CO_SUSPENDED;
    while (1) {
        if (me->co_main_state != XF_CO_DEAD) {
            xf_co_resume(me, me->co_main, arg, me->co_main_state);
        }
        xf_co_yield(me);
    }
    xf_co_end(me);
}
