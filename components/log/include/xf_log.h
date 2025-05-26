/**
 * @file xf_log.h
 * @author catcatBlue (catcatblue@qq.com)
 * @brief
 * @version 1.0
 * @date 2025-05-15
 *
 * SPDX-FileCopyrightText: 2025 CompanyNameMagicTag
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#ifndef __XF_LOG_H__
#define __XF_LOG_H__

/* ==================== [Includes] ========================================== */

#include <inttypes.h>

#include "xf_common.h"
#include "xf_check.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== [Defines] =========================================== */

#define XF_LOG_NONE             (0)
#define XF_LOG_USER             (1)
#define XF_LOG_ERROR            (2)
#define XF_LOG_WARN             (3)
#define XF_LOG_INFO             (4)
#define XF_LOG_DEBUG            (5)
#define XF_LOG_VERBOSE          (6)

#define XF_LOG_LEVEL            CONFIG_XF_LOG_LEVEL

/* ==================== [Typedefs] ========================================== */

/* ==================== [Global Prototypes] ================================= */

/**
 * @brief 日志等级转提示字符。
 *
 * @param level 日志等级。
 *      - XF_LOG_NONE       无
 *      - XF_LOG_USER       用户
 *      - XF_LOG_ERROR      错误
 *      - XF_LOG_WARN       警告
 *      - XF_LOG_INFO       信息
 *      - XF_LOG_DEBUG      调试
 *      - XF_LOG_VERBOSE    详细
 * @return char 提示字符。
 */
char xf_log_level_to_prompt(uint32_t level);

/* ==================== [Macros] ============================================ */

#if !defined(xf_log_printf)
#include <stdio.h>
#   define xf_log_printf(format, ...)   do { \
                                            printf(format, ##__VA_ARGS__); \
                                            fflush(stdout); \
                                        } while (0)
#endif

#if !defined(xf_log_timestamp)
#   define xf_log_timestamp()           ((uint64_t)0)
#endif

#if !defined(xf_log_level) && defined(xf_log_printf)
#   define xf_log_level(level, tag, format, ...) \
        do { \
            xf_log_printf( \
                "%c (%" PRIu64 ")-%s: " format "\r\n", \
                xf_log_level_to_prompt(level), xf_log_timestamp(), tag, \
                ##__VA_ARGS__ \
            ); \
        } while (0)
#endif

/* MODULE TAG */
#define MTAG                                xf_this_module
#define XF_MLOG_DEFINE_THIS_MODULE(_name)   static const char *const MTAG = (_name)
#define XF_MLOG_DEFINE_THIS_FILE()          static const char *const MTAG = (__FILENAME__)
#define XF_MLOG_DEFINE()                    XF_MLOG_DEFINE_THIS_FILE()

#if XF_LOG_LEVEL >= XF_LOG_USER
/**
 * @brief 用户等级日志。始终显示文件名、行号等信息。
 *
 * @param tag 日志标签。
 * @param format 日志格式化字符串。
 * @param ... 可变参数。
 * @return size_t 本次日志字节数。
 */
#   define XF_LOGU(tag, format, ...)    xf_log_level(XF_LOG_USER,     tag, format, ##__VA_ARGS__)
#   define XF_MLOGU(format, ...)        xf_log_level(XF_LOG_USER,    MTAG, format, ##__VA_ARGS__)
#else
#   define XF_LOGU(tag, format, ...)    UNUSED(tag)
#   define XF_MLOGU(format, ...)        UNUSED(MTAG)
#endif

#if XF_LOG_LEVEL >= XF_LOG_ERROR
/**
 * @brief 错误等级日志。始终显示文件名、行号等信息。
 *
 * @param tag 日志标签。
 * @param format 日志格式化字符串。
 * @param ... 可变参数。
 * @return size_t 本次日志字节数。
 */
#   define XF_LOGE(tag, format, ...)    xf_log_level(XF_LOG_ERROR,    tag, format, ##__VA_ARGS__)
#   define XF_MLOGE(format, ...)        xf_log_level(XF_LOG_ERROR,   MTAG, format, ##__VA_ARGS__)
#else
#   define XF_LOGE(tag, format, ...)    UNUSED(tag)
#   define XF_MLOGE(format, ...)        UNUSED(MTAG)
#endif

#if XF_LOG_LEVEL >= XF_LOG_WARN
/**
 * @brief 警告等级日志。
 *
 * @param tag 日志标签。
 * @param format 日志格式化字符串。
 * @param ... 可变参数。
 * @return size_t 本次日志字节数。
 */
#   define XF_LOGW(tag, format, ...)    xf_log_level(XF_LOG_WARN,     tag, format, ##__VA_ARGS__)
#   define XF_MLOGW(format, ...)        xf_log_level(XF_LOG_WARN,    MTAG, format, ##__VA_ARGS__)
#else
#   define XF_LOGW(tag, format, ...)    UNUSED(tag)
#   define XF_MLOGW(format, ...)        UNUSED(MTAG)
#endif

#if XF_LOG_LEVEL >= XF_LOG_INFO
/**
 * @brief 信息等级日志。
 *
 * @param tag 日志标签。
 * @param format 日志格式化字符串。
 * @param ... 可变参数。
 * @return size_t 本次日志字节数。
 */
#   define XF_LOGI(tag, format, ...)    xf_log_level(XF_LOG_INFO,     tag, format, ##__VA_ARGS__)
#   define XF_MLOGI(format, ...)        xf_log_level(XF_LOG_INFO,    MTAG, format, ##__VA_ARGS__)
#else
#   define XF_LOGI(tag, format, ...)    UNUSED(tag)
#   define XF_MLOGI(format, ...)        UNUSED(MTAG)
#endif

#if XF_LOG_LEVEL >= XF_LOG_DEBUG
/**
 * @brief 调试等级日志。
 *
 * @param tag 日志标签。
 * @param format 日志格式化字符串。
 * @param ... 可变参数。
 * @return size_t 本次日志字节数。
 */
#   define XF_LOGD(tag, format, ...)    xf_log_level(XF_LOG_DEBUG,    tag, format, ##__VA_ARGS__)
#   define XF_MLOGD(format, ...)        xf_log_level(XF_LOG_DEBUG,   MTAG, format, ##__VA_ARGS__)
#else
#   define XF_LOGD(tag, format, ...)    UNUSED(tag)
#   define XF_MLOGD(format, ...)        UNUSED(MTAG)
#endif

#if XF_LOG_LEVEL >= XF_LOG_VERBOSE
/**
 * @brief 冗余等级日志。
 *
 * @param tag 日志标签。
 * @param format 日志格式化字符串。
 * @param ... 可变参数。
 * @return size_t 本次日志字节数。
 */
#   define XF_LOGV(tag, format, ...)    xf_log_level(XF_LOG_VERBOSE,  tag, format, ##__VA_ARGS__)
#   define XF_MLOGV(format, ...)        xf_log_level(XF_LOG_VERBOSE, MTAG, format, ##__VA_ARGS__)
#else
#   define XF_LOGV(tag, format, ...)    UNUSED(tag)
#   define XF_MLOGV(format, ...)        UNUSED(MTAG)
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __XF_LOG_H__ */
