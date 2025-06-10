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
    co->temp.stimer = stimer;
    xf_co_resume(co, co, NULL, co_state);
    UNUSED(co_state);
}

xf_err_t xf_co_top_init(void)
{
    xf_co_attr_t co_attr = {0};
    if (sp_top->base.func == NULL) {
        co_attr.id = (XF_CO_NUM_MAX - 1);
        co_attr.type = XF_CO_TYPE_TOP;
        xf_co_ctor(xf_co_cast(&s_co_top_ctx), xf_co_func_cast(xf_co_top), NULL, &co_attr);
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
    xf_co_t *const co, xf_co_func_t func, void *user_data, xf_co_attr_t *p_attr)
{
    if ((!co) || (!func)) {
        return XF_ERR_INVALID_ARG;
    }
    xf_co_lc_init(xf_co_cast(co)->lc);
    xf_co_cast(co)->func = func;
#if CONFIG_XF_CO_ENABLE_USER_DATA
    xf_co_cast(co)->user_data = user_data;
#else
    UNUSED(user_data);
#endif
    xf_co_set_flags_state(co, XF_CO_SUSPENDED);
    xf_co_set_flags_await_bit(co, FALSE);
    /* 不改变 xf_co_set_flags_reserved */
    if (p_attr) {
        /* 检查此 id 是否已被占用 */
        if (NULL != xf_co_pool[p_attr->id].func) {
            XF_ERROR_LINE();
            return XF_FAIL;
        }
        xf_co_set_flags_type(co, p_attr->type);
        xf_co_set_flags_id(co, p_attr->id);
    } else {
        xf_co_set_flags_type(co, XF_CO_TYPE_UNKNOWN);
    }
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
    xf_co_func_t func, void *user_data, xf_co_attr_t *p_attr)
{
    int16_t i = 0;
    xf_err_t xf_ret;
    xf_co_attr_t attr = {0};
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
    if (NULL == p_attr) {
        attr.type = XF_CO_TYPE_UNKNOWN;
        attr.id = i;
        p_attr = &attr;
    }
    xf_ret = xf_co_ctor(&xf_co_pool[i], func, user_data, p_attr);
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
    id = xf_co_get_flags_id(co);
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
    xf_co_state_t res;
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
