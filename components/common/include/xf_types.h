/**
 * @file xf_types.h
 * @author catcatBlue (catcatblue@qq.com)
 * @brief
 * @version 1.0
 * @date 2025-05-14
 *
 * SPDX-FileCopyrightText: 2025 CompanyNameMagicTag
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#ifndef __XF_TYPES_H__
#define __XF_TYPES_H__

/* ==================== [Includes] ========================================== */

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== [Defines] =========================================== */

#if defined(_STDBOOL_H)
typedef bool bool_t;
#   if !defined(TRUE)
#       define TRUE     true
#   endif
#   if !defined(FALSE)
#       define FALSE    false
#   endif
#else
typedef uint8_t bool_t;
#   if !defined(TRUE)
#       define TRUE     (1U)
#   endif
#   if !defined(FALSE)
#       define FALSE    (0U)
#   endif
#endif /* defined(_STDBOOL_H) */

/* ==================== [Typedefs] ========================================== */

/* ==================== [Global Prototypes] ================================= */

/* ==================== [Macros] ============================================ */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* __XF_TYPES_H__ */
