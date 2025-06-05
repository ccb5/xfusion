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

__STATIC_INLINE xf_tick_t xf_tick_get_tick_freq(void);
__STATIC_INLINE xf_tick_t xf_tick_to_us(xf_tick_t tick);
__STATIC_INLINE xf_tick_t xf_tick_to_ms(xf_tick_t tick);
__STATIC_INLINE xf_tick_t xf_us_to_tick(xf_tick_t us);
__STATIC_INLINE xf_tick_t xf_ms_to_tick(xf_tick_t ms);
__STATIC_INLINE xf_tick_t xf_tick_get_us(void);
__STATIC_INLINE xf_tick_t xf_tick_get_ms(void);

__STATIC_INLINE xf_tick_t xf_tick_get_tick_freq(void)
{
    return CONFIG_XF_TICK_FREQ;
}

__STATIC_INLINE xf_tick_t xf_tick_to_us(xf_tick_t tick)
{
    return tick / xf_tick_get_tick_freq();
}

__STATIC_INLINE xf_tick_t xf_tick_to_ms(xf_tick_t tick)
{
    return (tick * 1000U) / xf_tick_get_tick_freq();
}

__STATIC_INLINE xf_tick_t xf_us_to_tick(xf_tick_t us)
{
    return us * xf_tick_get_tick_freq();
}

__STATIC_INLINE xf_tick_t xf_ms_to_tick(xf_tick_t ms)
{
    return (ms * xf_tick_get_tick_freq()) / 1000U;
}

__STATIC_INLINE xf_tick_t xf_tick_get_us(void)
{
    return (xf_tick_get_count() * 1000U * 1000U) / xf_tick_get_tick_freq();
}

__STATIC_INLINE xf_tick_t xf_tick_get_ms(void)
{
    return xf_tick_get_us() / 1000U;
}

/* ==================== [Macros] ============================================ */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __XF_TICK_H__ */
