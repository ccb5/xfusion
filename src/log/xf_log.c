/**
 * @file xf_log.c
 * @author catcatBlue (catcatblue@qq.com)
 * @brief
 * @version 1.0
 * @date 2025-05-15
 *
 * SPDX-FileCopyrightText: 2025 CompanyNameMagicTag
 * SPDX-License-Identifier: Apache-2.0
 *
 */

/* ==================== [Includes] ========================================== */

#include "xf_log.h"

/* ==================== [Defines] =========================================== */

/* ==================== [Typedefs] ========================================== */

/* ==================== [Static Prototypes] ================================= */

static const char s_lvl_to_prompt[] = {
    '\0',
    'E',
    'W',
    'I',
    'D',
    'V',
};

/* ==================== [Static Variables] ================================== */

/* ==================== [Macros] ============================================ */

/* ==================== [Global Functions] ================================== */

char xf_log_level_to_prompt(uint32_t level)
{
    if (level > XF_LOG_VERBOSE) {
        level = XF_LOG_NONE;
    }
    return s_lvl_to_prompt[level];
}

/* ==================== [Static Functions] ================================== */
