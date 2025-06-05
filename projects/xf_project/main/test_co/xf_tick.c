/**
 * @file xf_tick.c
 * @author catcatBlue (catcatblue@qq.com)
 * @brief 
 * @version 1.0
 * @date 2025-06-05
 * 
 * SPDX-FileCopyrightText: 2025 CompanyNameMagicTag
 * SPDX-License-Identifier: Apache-2.0
 * 
 */

/* ==================== [Includes] ========================================== */

#include "xf_tick.h"

/* ==================== [Defines] =========================================== */

/* ==================== [Typedefs] ========================================== */

/* ==================== [Static Prototypes] ================================= */

/* ==================== [Static Variables] ================================== */

static xf_tick_t s_sys_tick = 0U;
static volatile uint8_t tick_irq_flag;

/* ==================== [Macros] ============================================ */

/* ==================== [Global Functions] ================================== */

xf_tick_t xf_tick_inc(xf_tick_t tick)
{
    xf_tick_t sys_tick_prev = s_sys_tick;
    tick_irq_flag = 0U;
    s_sys_tick += tick;
    return sys_tick_prev;
}

xf_tick_t xf_tick_get_count(void)
{
    xf_tick_t result;
    do {
        tick_irq_flag = 1U;
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
        prev_tick = (~(xf_tick_t)0U) - prev_tick + 1U;
        prev_tick += act_time;
    }
    return prev_tick;
}

/* ==================== [Static Functions] ================================== */
