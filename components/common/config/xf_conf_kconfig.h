/**
 * @file xf_conf_kconfig.h
 * @author catcatBlue (catcatblue@qq.com)
 * @brief 兼容各平台 Kconfig 头文件。
 * @version 1.0
 * @date 2025-05-14
 *
 * SPDX-FileCopyrightText: 2025 CompanyNameMagicTag
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#ifndef __XF_CONF_KCONFIG_H__
#define __XF_CONF_KCONFIG_H__

/* ==================== [Includes] ========================================== */

#if defined(XF_CONF_KCONFIG_EXTERNAL_INCLUDE)
#    include XF_CONF_KCONFIG_EXTERNAL_INCLUDE
#else

#   if defined(XF_PLATFORM)
#       include "sdkconfig.h"
#   elif defined(ESP_PLATFORM)
#       include "sdkconfig.h"
#       include "esp_attr.h"
#   endif

#endif /* XF_CONF_KCONFIG_EXTERNAL_INCLUDE */

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== [Defines] =========================================== */

/* ==================== [Typedefs] ========================================== */

/* ==================== [Global Prototypes] ================================= */

/* ==================== [Macros] ============================================ */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __XF_CONF_KCONFIG_H__ */
