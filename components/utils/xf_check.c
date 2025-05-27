/**
 * @file xf_check.c
 * @author catcatBlue (catcatblue@qq.com)
 * @brief
 * @version 1.0
 * @date 2025-05-27
 *
 * SPDX-FileCopyrightText: 2025 CompanyNameMagicTag
 * SPDX-License-Identifier: Apache-2.0
 *
 */

/* ==================== [Includes] ========================================== */

#include "xf_check.h"

/* ==================== [Defines] =========================================== */

#define TAG "xf_check"

/* ==================== [Typedefs] ========================================== */

/* ==================== [Static Prototypes] ================================= */

/* ==================== [Static Variables] ================================== */

/* ==================== [Macros] ============================================ */

/* ==================== [Global Functions] ================================== */

__weak void xf_check_on_fatal_error(const char *file, int line)
{
    XF_LOGE(TAG, "FATAL_ERROR@%s:%d", file, line);
    while (1) {
    }
}

/* ==================== [Static Functions] ================================== */
