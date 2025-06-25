/**
 * @file xf_tick.c
 * @author catcatBlue (catcatblue@qq.com)
 * @brief
 * @version 1.0
 * @date 2025-06-05
 *
 * SPDX-FileCopyrightText: 2025 LVGL LLC
 * SPDX-License-Identifier: MIT licence
 *
 * SPDX-FileCopyrightText: 2025 CompanyNameMagicTag
 * SPDX-License-Identifier: Apache-2.0
 *
 */

/* ==================== [Includes] ========================================== */

#include "xf_tick.h"

#if XF_TICK_ENABLE_CUSTOM
#include XF_TICK_CUSTOM_INCLUDE
#endif

/* ==================== [Defines] =========================================== */

/* ==================== [Typedefs] ========================================== */

/* ==================== [Static Prototypes] ================================= */

/* ==================== [Static Variables] ================================== */

static xf_tick_t s_sys_tick = 0U;
static volatile uint8_t tick_irq_flag;
static xf_tick_get_cb_t s_get_tick_cb = NULL;
static xf_tick_delay_cb_t s_delay_cb = NULL;

/* ==================== [Macros] ============================================ */

/* ==================== [Global Functions] ================================== */

xf_tick_t xf_tick_inc(xf_tick_t tick)
{
    xf_tick_t sys_tick_prev = s_sys_tick;
    tick_irq_flag = 0;
    s_sys_tick += tick;
    return sys_tick_prev;
}

xf_tick_t xf_tick_get_count(void)
{
    xf_tick_t result = 0;
    if (s_get_tick_cb) {
        return s_get_tick_cb();
    }
    do {
        tick_irq_flag = 1;
        result        = s_sys_tick;
    } while (!tick_irq_flag);
    return result;
}

xf_tick_t xf_tick_elaps(xf_tick_t prev_tick)
{
    xf_tick_t act_time = xf_tick_get_count();
    if (act_time >= prev_tick) {
        prev_tick = act_time - prev_tick;
    } else {
        prev_tick = XF_TICK_MAX - prev_tick + 1;
        prev_tick += act_time;
    }
    return prev_tick;
}

void xf_tick_delay(xf_tick_t tick)
{
    if (s_delay_cb) {
        s_delay_cb(tick);
    } else {
        xf_tick_t start_tick = xf_tick_get_count();
        while (xf_tick_elaps(start_tick) < tick) {
            volatile uint32_t i;
            volatile uint32_t x = tick;
            for (i = 0; i < 100; i++) {
                x = x * 3;
            }
        }
    }
}

void xf_tick_set_tick_cb(xf_tick_get_cb_t cb)
{
    s_get_tick_cb = cb;
}

void xf_tick_set_delay_cb(xf_tick_delay_cb_t cb)
{
    s_delay_cb = cb;
}

/* ==================== [Static Functions] ================================== */
