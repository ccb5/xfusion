/**
 * @file xf_arithmetic.h
 * @author catcatBlue (catcatblue@qq.com)
 * @brief
 * @version 1.0
 * @date 2025-05-23
 *
 * SPDX-FileCopyrightText: 2025 CompanyNameMagicTag
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#ifndef __XF_ARITHMETIC_H__
#define __XF_ARITHMETIC_H__

/* ==================== [Includes] ========================================== */

#include "xf_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== [Defines] =========================================== */

#define XF_AM_POPCOUNT_U32_GUN  0
#define XF_AM_POPCOUNT_U32_V1   1
#define XF_AM_POPCOUNT_U32_V2   2

#define XF_AM_POPCOUNT_U32_ENABLE_MASK \
    (0x00 | BIT(XF_AM_POPCOUNT_U32_GUN) \
          | BIT(XF_AM_POPCOUNT_U32_V1) \
          | BIT(XF_AM_POPCOUNT_U32_V2))

#if !defined(XF_AM_POPCOUNT_U32_DEFAULT)
#define XF_AM_POPCOUNT_U32_DEFAULT  XF_AM_POPCOUNT_U32_V2
#endif

/* ==================== [Typedefs] ========================================== */

/* ==================== [Global Prototypes] ================================= */

#if XF_AM_POPCOUNT_U32_ENABLE_MASK & BIT(XF_AM_POPCOUNT_U32_GUN)
__STATIC_INLINE uint32_t xf_am_popcount_u32_gnu(uint32_t n)
{
    return __builtin_popcount(n);
}
#endif
uint32_t xf_am_popcount_u32_v1(uint32_t n);
uint32_t xf_am_popcount_u32_v2(uint32_t n);

uint32_t xf_am_log2_u32(uint32_t n);

/**
 * @brief 计算位元 1 的个数。(population count, 种群计数)
 *
 * @param n
 * @return uint32_t
 */
__STATIC_INLINE uint32_t xf_am_popcount_u32(uint32_t n)
{
#if     (XF_AM_POPCOUNT_U32_DEFAULT == XF_AM_POPCOUNT_U32_GUN)
    return xf_am_popcount_u32_gnu(n);
#elif   (XF_AM_POPCOUNT_U32_DEFAULT == XF_AM_POPCOUNT_U32_V1)
    return xf_am_popcount_u32_v1(n);
#elif   (XF_AM_POPCOUNT_U32_DEFAULT == XF_AM_POPCOUNT_U32_V2)
    return xf_am_popcount_u32_v2(n);
#endif
}

uint32_t xf_am_swap_u32(uint32_t n);
uint32_t xf_am_reverse_u32(uint32_t n);

uint32_t xf_am_clz_u32(uint32_t n);
uint32_t xf_am_ctz_u32(uint32_t n);

#define xf_am_fls_u32(_n)               (32U - xf_am_clz_u32(x))

/**
 * @brief 向下取整到 2 的幂。(floor power of 2)
 *
 * 获取小于等于 n 且最接近 n 的 2 的整数幂。
 *
 * @param n
 * @return uint32_t
 */
uint32_t xf_am_flp2_u32(uint32_t n);
__STATIC_INLINE uint32_t xf_am_round_down_to_power_of_2_u32(uint32_t n)
{
    return xf_am_flp2_u32(n);
}

/**
 * @brief 向上对齐到 2 的幂。(ceiling power of 2)
 *
 * 获取大于等于 n 且最接近 n 的 2 的整数幂。
 *
 * @param n
 * @return uint32_t
 */
uint32_t xf_am_clp2_u32(uint32_t n);
__STATIC_INLINE uint32_t xf_am_round_up_to_power_of_2_u32(uint32_t n)
{
    return xf_am_clp2_u32(n);
}

#if XF_COMMON_ENABLE_64BITS
uint32_t xf_am_clz_u64(uint64_t x);
#endif /* XF_COMMON_ENABLE_64BITS */

/* ==================== [Macros] ============================================ */

#define xf_align(x, a)                  ALIGN(x, a)
#define xf_log2(n)                      xf_am_log2_u32(n)
#define xf_bitswap32(n)                 xf_am_swap_u32(n)
#define xf_bitrev(n)                    xf_am_reverse_u32(n)
#define xf_popcount(n)                  xf_am_popcount_u32(n)
#define xf_clz(n)                       xf_am_clz_u32(n)

#define XF_AM_LOG2_STEP_16(n)           (((n) & 0xffff0000) ? 16 + XF_AM_LOG2_STEP_8((n) >> 16) : XF_AM_LOG2_STEP_8(n))
#define XF_AM_LOG2_STEP_8(n)            (((n) & 0x0000ff00) ?  8 + XF_AM_LOG2_STEP_4((n) >>  8) : XF_AM_LOG2_STEP_4(n))
#define XF_AM_LOG2_STEP_4(n)            (((n) & 0x000000f0) ?  4 + XF_AM_LOG2_STEP_2((n) >>  4) : XF_AM_LOG2_STEP_2(n))
#define XF_AM_LOG2_STEP_2(n)            (((n) & 0x0000000c) ?  2 + XF_AM_LOG2_STEP_1((n) >>  2) : XF_AM_LOG2_STEP_1(n))
#define XF_AM_LOG2_STEP_1(n)            (((n) & 0x00000002) ?  1 + XF_AM_LOG2_STEP_0((n) >>  1) : 0 )
#define XF_AM_LOG2_STEP_0(n)            (0)

#define xf_am_log2_u32_const(n)         XF_AM_LOG2_STEP_16(n)

#ifdef __cplusplus
}
#endif

#endif /* __XF_ARITHMETIC_H__ */
