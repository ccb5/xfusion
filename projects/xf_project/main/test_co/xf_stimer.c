/**
 * @file xf_stimer.c
 * @author catcatBlue (catcatblue@qq.com)
 * @brief
 * @version 1.0
 * @date 2025-06-04
 *
 * SPDX-FileCopyrightText: 2025 CompanyNameMagicTag
 * SPDX-License-Identifier: Apache-2.0
 *
 */

/* ==================== [Includes] ========================================== */

#include "xf_stimer.h"

/* ==================== [Defines] =========================================== */

#define TAG "xf_stimer"
#define IDLE_MEAS_PERIOD 500 /*!< 空闲测量周期，单位 [tick] */

/* ==================== [Typedefs] ========================================== */

/* ==================== [Static Prototypes] ================================= */

static bool_t xf_stimer_exec(xf_stimer_t *stimer);
static xf_tick_t xf_stimer_time_remaining(xf_stimer_t *stimer);
static xf_tick_t xf_stimer_get_min(xf_stimer_t **pp_stimer);

/* ==================== [Static Variables] ================================== */

/* 定时器池 */
static xf_stimer_t s_stimer_pool[XF_STIMER_NUM_MAX] = {0};
static xf_stimer_t *const sp_pool = s_stimer_pool;
/* 用于指示已使用的定时器 */
static xf_bitmap32_t s_stimer_bm[XF_BITMAP32_GET_BLK_SIZE(XF_STIMER_NUM_MAX)] = {0};
static xf_stimer_t *sp_stimer_min = NULL;  /*!< TODO 还需获取此指针的接口，或移除 */

static xf_tick_t s_idle_period_start = 0;
static xf_tick_t s_busy_time         = 0;

static uint8_t s_idle_last;
static bool_t stimer_created = FALSE;
static bool_t stimer_deleted = FALSE;

/* ==================== [Macros] ============================================ */

/* ==================== [Global Functions] ================================== */

xf_stimer_t *xf_stimer_acquire(void)
{
    int32_t idx;
    idx = xf_bitmap32_flz(s_stimer_bm, XF_STIMER_NUM_MAX);
    if (idx < 0) {
        return NULL;
    }
    XF_BITMAP32_SET1(s_stimer_bm, idx);
    stimer_created = TRUE;
    return &sp_pool[idx];
}

xf_err_t xf_stimer_release(xf_stimer_t *stimer)
{
    const xf_stimer_t stimer_empty = {0};
    intptr_t idx;
    if (stimer == NULL) {
        return XF_ERR_INVALID_ARG;
    }
    /* 检查是否是池内的定时器 */
    if ((stimer < &s_stimer_pool[0])
            || (stimer > &s_stimer_pool[XF_STIMER_NUM_MAX - 1])) {
        return XF_ERR_INVALID_ARG;
    }
    idx = ((intptr_t)stimer - (intptr_t)&s_stimer_pool[0]) / (sizeof(s_stimer_pool[0]));
    if (XF_BITMAP32_GET(s_stimer_bm, idx) == 0) {
        return XF_ERR_INVALID_ARG;
    }
    XF_BITMAP32_SET0(s_stimer_bm, idx);
    *stimer = stimer_empty;
    stimer_deleted = TRUE;
    return XF_OK;
}

xf_err_t xf_stimer_set_cb(xf_stimer_t *stimer, xf_stimer_cb_t cb_func)
{
    if ((stimer == NULL) || (cb_func == NULL)) {
        return XF_ERR_INVALID_ARG;
    }
    stimer->cb_func = cb_func;
    return XF_OK;
}

xf_err_t xf_stimer_set_user_data(xf_stimer_t *stimer, void *user_data)
{
    if (stimer == NULL) {
        return XF_ERR_INVALID_ARG;
    }
    stimer->user_data = user_data;
    return XF_OK;
}

xf_err_t xf_stimer_set_period(xf_stimer_t *stimer, xf_tick_t tick_period)
{
    if (stimer == NULL) {
        return XF_ERR_INVALID_ARG;
    }
    stimer->tick_period = tick_period;
    return XF_OK;
}

xf_err_t xf_stimer_set_repeat_count(xf_stimer_t *stimer, uint32_t repeat_count)
{
    if (stimer == NULL) {
        return XF_ERR_INVALID_ARG;
    }
    stimer->repeat_count = repeat_count;
    return XF_OK;
}

xf_err_t xf_stimer_init(xf_stimer_t *stimer)
{
    return xf_stimer_reset(stimer);
}

xf_err_t xf_stimer_reset(xf_stimer_t *stimer)
{
    if (stimer == NULL) {
        return XF_ERR_INVALID_ARG;
    }
    stimer->tick_last_run = xf_tick_get_count();
    return XF_OK;
}

xf_err_t xf_stimer_set_ready(xf_stimer_t *stimer)
{
    if (stimer == NULL) {
        return XF_ERR_INVALID_ARG;
    }
    stimer->tick_last_run = xf_tick_get_count() - stimer->tick_period - 1;
    return XF_OK;
}

xf_stimer_t *xf_stimer_create(
    xf_tick_t tick_period, xf_stimer_cb_t cb_func, void *user_data)
{
    xf_err_t xf_err = XF_OK;
    xf_stimer_t *stimer = xf_stimer_acquire();
    if (stimer == NULL) {
        return NULL;
    }
    xf_err |= xf_stimer_init(stimer);
    xf_err |= xf_stimer_set_cb(stimer, cb_func);
    xf_err |= xf_stimer_set_user_data(stimer, user_data);
    xf_err |= xf_stimer_set_period(stimer, tick_period);
    xf_err |= xf_stimer_set_repeat_count(stimer, XF_STIMER_INFINITY);
    return stimer;
}

xf_err_t xf_stimer_destroy(xf_stimer_t *stimer)
{
    return xf_stimer_release(stimer);
}

xf_tick_t xf_stimer_handler(void)
{
    int32_t stimer_idx;
    xf_tick_t tick_min;
    xf_tick_t idle_period_time;
    /* cppcheck-suppress misra-c2012-18.8 */
    xf_bitmap32_t stimer_bm_temp[XF_BITMAP32_GET_BLK_SIZE(XF_STIMER_NUM_MAX)];
    xf_tick_t handler_start = xf_tick_get_count();

    /* 检查是否正常调用 xf_tick_inc */
    if (handler_start == 0) {
        static uint8_t s_run_cnt = 0;
        s_run_cnt++;
        if (s_run_cnt > 100U) {
            s_run_cnt = 0U;
            XF_LOGE(TAG, "It seems xf_tick_inc() is not called.");
            XF_FATAL_ERROR();
        }
    }

    /* 如果运行过程中有定时器创建或移除，则重新检测所有定时器是否执行。 */
    do {
        memcpy(stimer_bm_temp, s_stimer_bm, sizeof(s_stimer_bm));
        stimer_deleted = FALSE;
        stimer_created = FALSE;
        stimer_idx = xf_bitmap32_fls(stimer_bm_temp, XF_STIMER_NUM_MAX);
        while (stimer_idx >= 0) {
            if (xf_stimer_exec(&sp_pool[stimer_idx])) {
                if (stimer_created || stimer_deleted) {
                    break;
                }
            }
            XF_BITMAP32_SET0(stimer_bm_temp, stimer_idx);
            stimer_idx = xf_bitmap32_fls(stimer_bm_temp, XF_STIMER_NUM_MAX);
        }
    } while (stimer_idx >= 0);

    /* 获取下一次唤醒的最小时间 */
    tick_min = xf_stimer_get_min(&sp_stimer_min);

    /* 统计空闲时间 */
    s_busy_time += xf_tick_elaps(handler_start);
    idle_period_time = xf_tick_elaps(s_idle_period_start);
    if (idle_period_time >= IDLE_MEAS_PERIOD) {
        s_idle_last         = (uint8_t)((s_busy_time * 100U) / idle_period_time);
        s_idle_last         = (s_idle_last > 100U) ? (0U) : (100U - s_idle_last);
        s_busy_time         = 0U;
        s_idle_period_start = xf_tick_get_count();
    }

    return tick_min;
}

uint8_t xf_stimer_get_idle_percentage(void)
{
    return s_idle_last;
}

/* ==================== [Static Functions] ================================== */

static bool_t xf_stimer_exec(xf_stimer_t *stimer)
{
    bool_t exec = FALSE;
    if (xf_stimer_time_remaining(stimer) == 0U) {
        uint32_t original_repeat_count = stimer->repeat_count;
        if ((stimer->repeat_count != XF_STIMER_INFINITY)
                && (stimer->repeat_count != 0)) {
            --stimer->repeat_count;
        }
        stimer->tick_last_run = xf_tick_get_count();
        if ((stimer->cb_func) && (original_repeat_count != 0U)) {
            stimer->cb_func(stimer);
        }
        exec = TRUE;
    }
    if (stimer_deleted == FALSE) {
        if (stimer->repeat_count == 0) {
            xf_stimer_release(stimer);
        }
    }
    return exec;
}

static xf_tick_t xf_stimer_time_remaining(xf_stimer_t *stimer)
{
    xf_tick_t elp = xf_tick_elaps(stimer->tick_last_run);
    if (elp >= stimer->tick_period) {
        return 0;
    }
    return stimer->tick_period - elp;
}

static xf_tick_t xf_stimer_get_min(xf_stimer_t **pp_stimer)
{
    int32_t i;
    xf_tick_t tick_min;
    for (i = 0; i < (int32_t)XF_STIMER_NUM_MAX; i++) {
        /* 先找首个有效定时器 */
        if (XF_BITMAP32_GET(s_stimer_bm, i) != 0U) {
            tick_min = xf_stimer_time_remaining(&sp_pool[i]);
            break;
        }
    }
    if (i != (int32_t)XF_STIMER_NUM_MAX) {
        int32_t j; /*!< j: 最小时间定时事件所在索引 */
        xf_tick_t tick_temp;
        j = i;
        for (i = i + 1; i < (int32_t)XF_STIMER_NUM_MAX; i++) {
            if (XF_BITMAP32_GET(s_stimer_bm, i) != 0U) {
                tick_temp = xf_stimer_time_remaining(&sp_pool[j]);
                if (tick_min < tick_temp) {
                    tick_min = tick_temp;
                    j = i;
                }
            }
        }
        if (pp_stimer) {
            *pp_stimer = &sp_pool[j];
        }
        return tick_min;
    }
    return XF_STIMER_NO_READY;
}
