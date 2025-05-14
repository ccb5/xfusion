/**
 * @file xf_conf_internal.h
 * @author catcatBlue (catcatblue@qq.com)
 * @brief
 * @version 1.0
 * @date 2025-05-14
 */

/*
 * SPDX-FileCopyrightText: 2025 CompanyNameMagicTag
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __XF_CONF_INTERNAL_H__
#define __XF_CONF_INTERNAL_H__

/* ==================== [Includes] ========================================== */

/* Handle special Kconfig options */
#if !defined(XF_KCONFIG_IGNORE)
#    include "xf_conf_kconfig.h"
#    ifdef CONFIG_XF_CONF_SKIP
#        define XF_CONF_SKIP
#    endif
#endif

/*If xf_conf.h is not skipped include it*/
#if !defined(XF_CONF_SKIP) || defined(XF_CONF_PATH)
#   if defined(XF_CONF_PATH)                    /* If there is a path defined for xf_conf.h use it */
#       define __XF_TO_STR_AUX(x) #x
#       define __XF_TO_STR(x) __XF_TO_STR_AUX(x)
#       include __XF_TO_STR(XF_CONF_PATH)
#       undef __XF_TO_STR_AUX
#       undef __XF_TO_STR
#   elif defined(XF_CONF_INCLUDE_SIMPLE)        /* Or simply include xf_conf.h is enabled */
#       include "xf_conf.h"
#   else
#       include "../../xf_conf.h"               /* Else assume xf_conf.h is next to the xf folder */
#   endif
#endif

#if !defined(XF_CONF_SKIP) && !defined(__XF_CONF_H__)
/* If "xf_conf.h" is available from here try to use it later. */
#   if defined(__has_include)
#       if __has_include("xf_conf.h")
#           if !defined(XF_CONF_INCLUDE_SIMPLE)
#               define XF_CONF_INCLUDE_SIMPLE
#           endif
#       endif
#   endif
#   if !defined(XF_CONF_SUPPRESS_DEFINE_CHECK)
/* #include will sometimes silently fail when __has_include is used */
/* https://gcc.gnu.org/bugzilla/show_bug.cgi?id=80753 */
#       pragma message("Possible failure to include xf_conf.h, please read the comment in this file if you get errors")
#   endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== [Defines] =========================================== */

#define XF_TODO                         0

#if !(defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L))
#   error "需要 C99 或以上版本的编译器"
#endif

#if defined(__GNUC__)
#   define XF_COM_USE_GNU               1
#   define XF_COM_USE_BUILTIN           1
#else
#   define XF_COM_USE_GNU               0
#   define XF_COM_USE_BUILTIN           0
#endif

/* ==================== [Typedefs] ========================================== */

/* ==================== [Global Prototypes] ================================= */

/* ==================== [Macros] ============================================ */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __XF_CONF_INTERNAL_H__ */
