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

#if !XF_LOG_ENABLE_CUSTOM_PORTING

#include <stdio.h>
#include <stdarg.h>

#endif /* !XF_LOG_ENABLE_CUSTOM_PORTING */

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

char xf_log_level_to_prompt(uint8_t level)
{
    if (level > XF_LOG_VERBOSE) {
        level = XF_LOG_NONE;
    }
    return s_lvl_to_prompt[level];
}

#if !XF_LOG_ENABLE_CUSTOM_PORTING

__IMPL void xf_log_printf(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

__IMPL void xf_log_level(char level, const char *tag, const char *format, ...)
{
    va_list args;
    xf_log_printf("%c (%lu)-%s: ",
                  xf_log_level_to_prompt(level),
                  (unsigned long int)xf_tick_to_ms(xf_tick_get_count()), tag);
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    xf_log_printf("\r\n");
}

#endif /* XF_LOG_ENABLE_CUSTOM_PORTING */

/* ==================== [Static Functions] ================================== */
