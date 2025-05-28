/**
 * @file xf_arithmetic.c
 * @author catcatBlue (catcatblue@qq.com)
 * @brief
 * @version 1.0
 * @date 2025-05-23
 *
 * SPDX-FileCopyrightText: 2025 CompanyNameMagicTag
 * SPDX-License-Identifier: Apache-2.0
 *
 */

/* ==================== [Includes] ========================================== */

#include "xf_arithmetic.h"

/* ==================== [Defines] =========================================== */

/* ==================== [Typedefs] ========================================== */

/* ==================== [Static Variables] ================================== */

/* ==================== [Global Variables] ================================== */

/* ==================== [Static Prototypes] ================================= */

/* ==================== [Macros] ============================================ */

/* ==================== [Global Functions] ================================== */

uint32_t xf_am_log2_u32(uint32_t n)
{
    uint32_t result = 0;
/* *INDENT-OFF* */
    if (n & 0xffff0000) { result += 16; n >>= 16; }
    if (n & 0x0000ff00) { result +=  8; n >>=  8; }
    if (n & 0x000000f0) { result +=  4; n >>=  4; }
    if (n & 0x0000000c) { result +=  2; n >>=  2; }
    if (n & 0x00000002) { result +=  1; /* n >>=  1; */ }
/* *INDENT-ON* */ 
    return result;
}

#if     (XF_AM_POPCOUNT_U32_ENABLE_MASK & BIT(XF_AM_POPCOUNT_U32_V1))
uint32_t xf_am_popcount_u32_v1(uint32_t n)
{
    n = (n & 0x55555555) + ((n >>  1) & 0x55555555);
    n = (n & 0x33333333) + ((n >>  2) & 0x33333333);
    n = (n & 0x0F0F0F0F) + ((n >>  4) & 0x0F0F0F0F);
    n = (n & 0x00FF00FF) + ((n >>  8) & 0x00FF00FF);
    n = (n & 0x0000FFFF) + ((n >> 16) & 0x0000FFFF);
    return n;
}
#endif

#if     (XF_AM_POPCOUNT_U32_ENABLE_MASK & BIT(XF_AM_POPCOUNT_U32_V2))
uint32_t xf_am_popcount_u32_v2(uint32_t n)
{
    n = n - ((n >> 1) & 0x55555555);
    n = (n & 0x33333333) + ((n >> 2) & 0x33333333);
    n = (n + (n >> 4)) & 0x0F0F0F0F;
    n = n + (n >> 8);
    n = n + (n >> 16);
    return n & 0x0000003F;
}
#endif

uint32_t xf_am_clz_u32(uint32_t n)
{
#if XF_COM_USE_GNU /* (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 5) */
    if (!n) {
        return 32;
    }
    return __builtin_clz(n);
#else
    int result = 0;
    if (n == 0) {
        return 32;
    }
/* *INDENT-OFF* */
   if (n <= 0x0000FFFF) { result = result + 16; n = n << 16; }
   if (n <= 0x00FFFFFF) { result = result +  8; n = n <<  8; }
   if (n <= 0x0FFFFFFF) { result = result +  4; n = n <<  4; }
   if (n <= 0x3FFFFFFF) { result = result +  2; n = n <<  2; }
   if (n <= 0x7FFFFFFF) { result = result +  1;              }
/* *INDENT-ON* */
    return result;
#endif
/* *INDENT-ON* */
}

uint32_t xf_am_ctz_u32(uint32_t n)
{
#if XF_COM_USE_GNU /* (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 5) */
    if (!n) {
        return 32;
    }
    return __builtin_ctz(n);
#else
    if (n == 0) {
        return 32;
    }
    int tmp = 1;
/* *INDENT-OFF* */
   if ((n & 0x0000FFFF) == 0) { tmp = tmp + 16; n = n >> 16; }
   if ((n & 0x000000FF) == 0) { tmp = tmp +  8; n = n >>  8; }
   if ((n & 0x0000000F) == 0) { tmp = tmp +  4; n = n >>  4; }
   if ((n & 0x00000003) == 0) { tmp = tmp +  2; n = n >>  2; }
/* *INDENT-ON* */
    return tmp - (n & 1);
#endif
/* *INDENT-ON* */
}

uint32_t xf_am_swap_u32(uint32_t n)
{
#if XF_COM_USE_GNU /* (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 5) */
    return __builtin_bswap32(n);
#else
    n = (n & 0xF0F0F0F0) >> 4 | (n & 0x0F0F0F0F) << 4;
    n = (n & 0xCCCCCCCC) >> 2 | (n & 0x33333333) << 2;
    n = (n & 0xAAAAAAAA) >> 1 | (n & 0x55555555) << 1;
    return n;
#endif
}

uint32_t xf_am_reverse_u32(uint32_t n)
{
    n = (n & 0x55555555) <<  1 | (n & 0xAAAAAAAA) >>  1;
    n = (n & 0x33333333) <<  2 | (n & 0xCCCCCCCC) >>  2;
    n = (n & 0x0F0F0F0F) <<  4 | (n & 0xF0F0F0F0) >>  4;
    n = (n & 0x00FF00FF) <<  8 | (n & 0xFF00FF00) >>  8;
    n = (n & 0x0000FFFF) << 16 | (n & 0xFFFF0000) >> 16;
    return n;
}

uint32_t xf_am_flp2_u32(uint32_t n)
{
    n = n | (n >> 1);
    n = n | (n >> 2);
    n = n | (n >> 4);
    n = n | (n >> 8);
    n = n | (n >> 16);
    return n - (n >> 1);
}

uint32_t xf_am_clp2_u32(uint32_t n)
{
    n = n - 1;
    n = n | (n >> 1);
    n = n | (n >> 2);
    n = n | (n >> 4);
    n = n | (n >> 8);
    n = n | (n >> 16);
    return n + 1;
}

/* ==================== [Static Functions] ================================== */
