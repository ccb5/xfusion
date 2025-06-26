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

#include "../common/xf_common.h"
#include "../system/tick/xf_tick.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== [Defines] =========================================== */

#define XF_LOG_NONE             (0)
#define XF_LOG_ERROR            (1)
#define XF_LOG_WARN             (2)
#define XF_LOG_INFO             (3)
#define XF_LOG_DEBUG            (4)
#define XF_LOG_VERBOSE          (5)

/* ==================== [Typedefs] ========================================== */

/* ==================== [Global Prototypes] ================================= */

/**
 * @brief 日志等级转提示字符。
 *
 * @param level 日志等级。
 *      - XF_LOG_NONE       无
 *      - XF_LOG_ERROR      错误
 *      - XF_LOG_WARN       警告
 *      - XF_LOG_INFO       信息
 *      - XF_LOG_DEBUG      调试
 *      - XF_LOG_VERBOSE    详细
 * @return char 提示字符。
 */
char xf_log_level_to_prompt(uint8_t level);

/* ==================== [Macros] ============================================ */

__EXT_IMPL void xf_log_printf(const char *format, ...);
__EXT_IMPL void xf_log_level(char level, const char *tag, const char *format, ...);

/* MODULE TAG */
#define MTAG                                xf_this_module
#define XF_MLOG_DEFINE_THIS_MODULE(_name)   static const char *const MTAG = (_name)
#define XF_MLOG_DEFINE_THIS_FILE()          static const char *const MTAG = (__FILENAME__)
#define XF_MLOG_DEFINE()                    XF_MLOG_DEFINE_THIS_FILE()

#if XF_LOG_ENABLE_ERROR_LEVEL
/**
 * @brief 错误等级日志。始终显示文件名、行号等信息。
 *
 * @param tag 日志标签。
 * @param format 日志格式化字符串。
 * @param ... 可变参数。
 * @return size_t 本次日志字节数。
 */
#   define XF_LOGE(tag, format, ...)    xf_log_level(XF_LOG_ERROR,  tag,  format, ##__VA_ARGS__)
#   define XF_MLOGE(format, ...)        xf_log_level(XF_LOG_ERROR,  MTAG, format, ##__VA_ARGS__)
#else
#   define XF_LOGE(tag, format, ...)    UNUSED(tag)
#   define XF_MLOGE(format, ...)        UNUSED(MTAG)
#endif

#if XF_LOG_ENABLE_WARN_LEVEL
/**
 * @brief 警告等级日志。
 *
 * @param tag 日志标签。
 * @param format 日志格式化字符串。
 * @param ... 可变参数。
 * @return size_t 本次日志字节数。
 */
#   define XF_LOGW(tag, format, ...)    xf_log_level(XF_LOG_WARN,  tag,  format, ##__VA_ARGS__)
#   define XF_MLOGW(format, ...)        xf_log_level(XF_LOG_WARN,  MTAG, format, ##__VA_ARGS__)
#else
#   define XF_LOGW(tag, format, ...)    UNUSED(tag)
#   define XF_MLOGW(format, ...)        UNUSED(MTAG)
#endif

#if XF_LOG_ENABLE_INFO_LEVEL
/**
 * @brief 信息等级日志。
 *
 * @param tag 日志标签。
 * @param format 日志格式化字符串。
 * @param ... 可变参数。
 * @return size_t 本次日志字节数。
 */
#   define XF_LOGI(tag, format, ...)    xf_log_level(XF_LOG_INFO,  tag,  format, ##__VA_ARGS__)
#   define XF_MLOGI(format, ...)        xf_log_level(XF_LOG_INFO,  MTAG, format, ##__VA_ARGS__)
#else
#   define XF_LOGI(tag, format, ...)    UNUSED(tag)
#   define XF_MLOGI(format, ...)        UNUSED(MTAG)
#endif

#if XF_LOG_ENABLE_DEBUG_LEVEL
/**
 * @brief 调试等级日志。
 *
 * @param tag 日志标签。
 * @param format 日志格式化字符串。
 * @param ... 可变参数。
 * @return size_t 本次日志字节数。
 */
#   define XF_LOGD(tag, format, ...)    xf_log_level(XF_LOG_DEBUG,  tag,  format, ##__VA_ARGS__)
#   define XF_MLOGD(format, ...)        xf_log_level(XF_LOG_DEBUG,  MTAG, format, ##__VA_ARGS__)
#else
#   define XF_LOGD(tag, format, ...)    UNUSED(tag)
#   define XF_MLOGD(format, ...)        UNUSED(MTAG)
#endif

#if XF_LOG_ENABLE_VERBOSE_LEVEL
/**
 * @brief 冗余等级日志。
 *
 * @param tag 日志标签。
 * @param format 日志格式化字符串。
 * @param ... 可变参数。
 * @return size_t 本次日志字节数。
 */
#   define XF_LOGV(tag, format, ...)    xf_log_level(XF_LOG_VERBOSE, tag,  format, ##__VA_ARGS__)
#   define XF_MLOGV(format, ...)        xf_log_level(XF_LOG_VERBOSE, MTAG, format, ##__VA_ARGS__)
#else
#   define XF_LOGV(tag, format, ...)    UNUSED(tag)
#   define XF_MLOGV(format, ...)        UNUSED(MTAG)
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __XF_LOG_H__ */
