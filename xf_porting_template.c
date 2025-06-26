/**
 * @file xf_porting_template.c
 * @author catcatBlue (catcatblue@qq.com)
 * @brief
 * @version 1.0
 * @date 2025-06-26
 *
 * SPDX-FileCopyrightText: 2025 CompanyNameMagicTag
 * SPDX-License-Identifier: Apache-2.0
 *
 */

/* ==================== [Includes] ========================================== */

#include <stdio.h>
#include <stdarg.h>

#include "xtiny.h"
#include "xf_porting.h"

/* ==================== [Defines] =========================================== */

/* ==================== [Typedefs] ========================================== */

/* ==================== [Static Prototypes] ================================= */

/* ==================== [Static Variables] ================================== */

/* ==================== [Macros] ============================================ */

/* ==================== [Global Functions] ================================== */

__IMPL void xf_log_printf(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    fflush(stdout);
}

__IMPL void xf_log_level(char level, const char *tag, const char *format, ...)
{
    va_list args;
    xf_log_printf("%c (%lu)-%s: ",
                  xf_log_level_to_prompt(level),
                  xf_tick_to_ms(xf_tick_get_count()),
                  tag);
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    xf_log_printf("\r\n");
    fflush(stdout);
}

/* ==================== [Static Functions] ================================== */
