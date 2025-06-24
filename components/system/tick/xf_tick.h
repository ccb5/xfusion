/**
 * @file xf_tick.h
 * @author catcatBlue (catcatblue@qq.com)
 * @brief
 * @version 1.0
 * @date 2025-06-05
 *
 * SPDX-FileCopyrightText: 2025 CompanyNameMagicTag
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#ifndef __XF_TICK_H__
#define __XF_TICK_H__

/* ==================== [Includes] ========================================== */

#include "xf_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== [Defines] =========================================== */

#define CONFIG_XF_TICK_FREQ             (1000U)

/* ==================== [Typedefs] ========================================== */

typedef uint32_t xf_tick_t;

/* ==================== [Global Prototypes] ================================= */

xf_tick_t xf_tick_inc(xf_tick_t tick);
xf_tick_t xf_tick_get_count(void);
xf_tick_t xf_tick_elaps(xf_tick_t prev_tick);

#define xf_tick_get_tick_freq()         (CONFIG_XF_TICK_FREQ)
#define xf_tick_to_us(_tick)            (((_tick) * 1000000U) / (CONFIG_XF_TICK_FREQ))
#define xf_tick_to_ms(_tick)            (((_tick) * 1000U) / (CONFIG_XF_TICK_FREQ))
#define xf_us_to_tick(_us)              ((_us) * (CONFIG_XF_TICK_FREQ) / (1000000U))
#define xf_ms_to_tick(_ms)              (((_ms) * (CONFIG_XF_TICK_FREQ)) / (1000U))
#define xf_tick_get_us()                ((xf_tick_get_count() * 1000000U) / (CONFIG_XF_TICK_FREQ))
#define xf_tick_get_ms()                ((xf_tick_get_count() * 1000U) / (CONFIG_XF_TICK_FREQ))

/* ==================== [Macros] ============================================ */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __XF_TICK_H__ */
