/**
 * @file xf_tick.h
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

#ifndef __XF_TICK_H__
#define __XF_TICK_H__

/* ==================== [Includes] ========================================== */

#include "../../utils/xf_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== [Defines] =========================================== */

/* ==================== [Typedefs] ========================================== */

typedef uint32_t xf_tick_t;
#define XF_TICK_MAX ((xf_tick_t)~(xf_tick_t)0)

typedef xf_tick_t (*xf_tick_get_cb_t)(void);

typedef void (*xf_tick_delay_cb_t)(xf_tick_t tick);

/* ==================== [Global Prototypes] ================================= */

xf_tick_t xf_tick_inc(xf_tick_t tick);
xf_tick_t xf_tick_get_count(void);
xf_tick_t xf_tick_elaps(xf_tick_t prev_tick);

/* 阻塞 delay ，小心使用！如果未设 tick_cb/delay_cb 且不在中断中调用 xf_tick_inc() 将死机 */
void xf_tick_delay(xf_tick_t tick);

void xf_tick_set_tick_cb(xf_tick_get_cb_t cb);
void xf_tick_set_delay_cb(xf_tick_delay_cb_t cb);

/* ==================== [Macros] ============================================ */

#define xf_tick_get_tick_freq()         (XF_TICK_FREQ)
#define xf_tick_to_us(_tick)            (((_tick) * 1000000U) / xf_tick_get_tick_freq())
#define xf_tick_to_ms(_tick)            (((_tick) * 1000U) / xf_tick_get_tick_freq())
#define xf_us_to_tick(_us)              ((_us) * xf_tick_get_tick_freq() / (1000000U))
#define xf_ms_to_tick(_ms)              (((_ms) * xf_tick_get_tick_freq()) / (1000U))
#define xf_tick_get_us()                ((xf_tick_get_count() * 1000000U) / xf_tick_get_tick_freq())
#define xf_tick_get_ms()                ((xf_tick_get_count() * 1000U) / xf_tick_get_tick_freq())

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __XF_TICK_H__ */
