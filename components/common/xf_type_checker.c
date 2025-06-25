/**
 * @file xf_type_checker.c
 * @author catcatBlue (catcatblue@qq.com)
 * @brief
 * @version 1.0
 * @date 2025-05-14
 *
 * SPDX-FileCopyrightText: 2025 CompanyNameMagicTag
 * SPDX-License-Identifier: Apache-2.0
 *
 */

/* ==================== [Includes] ========================================== */

#include "xf_common.h"

/* ==================== [Defines] =========================================== */

/* ==================== [Typedefs] ========================================== */

/* ==================== [Static Prototypes] ================================= */

/* ==================== [Static Variables] ================================== */

/* ==================== [Macros] ============================================ */

/* ==================== [Global Functions] ================================= */

STATIC_ASSERT(sizeof(int8_t)        == 1);
STATIC_ASSERT(sizeof(uint8_t)       == 1);

STATIC_ASSERT(sizeof(int16_t)       == 2);
STATIC_ASSERT(sizeof(uint16_t)      == 2);

STATIC_ASSERT(sizeof(int32_t)       == 4);
STATIC_ASSERT(sizeof(uint32_t)      == 4);


STATIC_ASSERT(sizeof(int64_t)       == 8);
STATIC_ASSERT(sizeof(uint64_t)      == 8);

STATIC_ASSERT(sizeof(intptr_t)      == sizeof(void *));
STATIC_ASSERT(sizeof(uintptr_t)     == sizeof(void *));

#if XF_TODO
// STATIC_ASSERT(sizeof(ptrdiff_t)     == sizeof(void *));
// STATIC_ASSERT(sizeof(xf_atomic_t)   == sizeof(void *));
// STATIC_ASSERT(sizeof(xf_atomic_t)   == sizeof(xf_atomic_val_t));
#endif /* XF_TODO */

STATIC_ASSERT(sizeof(size_t)        != 0);

STATIC_ASSERT(sizeof(1UL)           >= 4);

#if XF_COMMON_ENABLE_64BITS
STATIC_ASSERT(sizeof(1ULL)          >= 8);
#endif

#if XF_TODO
/*
    TODO ssize_t 在 #include <sys/types.h> 内定义.
    暂不使用.
 */
// STATIC_ASSERT(sizeof(ssize_t)       != 0);

STATIC_ASSERT(sizeof(xf_ret_info_t) == sizeof(xf_err_t));
STATIC_ASSERT(_XF_ERR_COM_MAX <= BIT_MASK(XF_RET_COMMON_ERRNO_BITSIZE));
STATIC_ASSERT((XF_RET_MODULE_ERRNO_BITSIZE
               + XF_RET_LINE_BITSIZE
               + XF_RET_FLAG_BITSIZE) <= 32);
#endif /* XF_TODO */

/* ==================== [Static Functions] ================================== */
