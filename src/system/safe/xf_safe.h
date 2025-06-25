/**
 * @file xf_safe.h
 * @author catcatBlue (catcatblue@qq.com)
 * @brief
 * @version 1.0
 * @date 2025-06-20
 *
 * SPDX-FileCopyrightText: 2025 CompanyNameMagicTag
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#ifndef __XF_SAFE_H__
#define __XF_SAFE_H__

/* ==================== [Includes] ========================================== */

#include "xf_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== [Defines] =========================================== */

#define XF_CRIT_PTR_UNINIT              ((void *)(uintptr_t)0xDEADBEEF)

/* ==================== [Typedefs] ========================================== */

/* ==================== [Global Prototypes] ================================= */

/* ==================== [Macros] ============================================ */

#if !defined(XF_CRIT_STAT)
#   define XF_CRIT_STAT()               ((void)0)
#endif

#if !defined(XF_CRIT_ENTRY)
#   define XF_CRIT_ENTRY()              ((void)0)
#endif

#if !defined(XF_CRIT_EXIT)
#   define XF_CRIT_EXIT()               ((void)0)
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __XF_SAFE_H__ */
