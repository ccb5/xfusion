/**
 * GENERATED FILE, DO NOT EDIT IT!
 * @file xf_conf_internal.h
 * This file ensures all defines of xf_conf.h have a default value.
 */

/* *INDENT-OFF* */

#ifndef __XF_CONF_INTERNAL_H__
#define __XF_CONF_INTERNAL_H__

/* Handle special Kconfig options */
#if !defined(XF_KCONFIG_IGNORE)
    #include "xf_conf_kconfig.h"
    #if defined(CONFIG_XF_CONF_SKIP) && !defined(XF_CONF_SKIP)
        #define XF_CONF_SKIP
    #endif
#endif

/* If "xf_conf.h" is available from here try to use it later. */
#ifdef __has_include
    #if __has_include("xf_conf.h")
        #ifndef XF_CONF_INCLUDE_SIMPLE
            #define XF_CONF_INCLUDE_SIMPLE
        #endif
    #endif
#endif

/* If xf_conf.h is not skipped include it */
#if !defined(XF_CONF_SKIP) || defined(XF_CONF_PATH)
    #ifdef XF_CONF_PATH                           /* If there is a path defined for xf_conf.h, use it */
        #include XF_CONF_PATH                     /* Note: Make sure to define custom CONF_PATH as a string */
    #elif defined(XF_CONF_INCLUDE_SIMPLE)         /* Or simply include xf_conf.h is enabled. */
        #include "xf_conf.h"
    #else
        #include "../../xf_conf.h"                /* Else assume xf_conf.h is next to the xtiny folder. */
    #endif
    #if !defined(__XF_CONF_H__) && !defined(XF_CONF_SUPPRESS_DEFINE_CHECK)
        /* #include will sometimes silently fail when __has_include is used */
        /* https://gcc.gnu.org/bugzilla/show_bug.cgi?id=80753 */
        #pragma message("Possible failure to include xf_conf.h, please read the comment in this file if you get errors")
    #endif
#endif

#if defined(CONFIG_XF_CONF_SKIP)
    #define XF_KCONFIG_PRESENT
#endif

/*----------------------------------
 * Start parsing xf_conf_template.h
 -----------------------------------*/

/* If you need to include anything here, do it inside the `__ASSEMBLY__` guard */
#if  0
#include "my_include.h"
#endif

/*  组件划分单位：内含 `CMakeLists.txt` 视为独立组件。 */

/* -------------------- components ------------------------------------------ */

/* -------------------- components/algo ------------------------------------- */

/* -------------------- components/common ----------------------------------- */

#ifndef XF_COMMON_ENABLE_64BITS
    #ifdef XF_KCONFIG_PRESENT
        #ifdef CONFIG_XF_COMMON_ENABLE_64BITS
            #define XF_COMMON_ENABLE_64BITS CONFIG_XF_COMMON_ENABLE_64BITS
        #else
            #define XF_COMMON_ENABLE_64BITS 0
        #endif
    #else
        #define XF_COMMON_ENABLE_64BITS             1
    #endif
#endif
#ifndef XF_COMMON_ENABLE_GNU
    #ifdef XF_KCONFIG_PRESENT
        #ifdef CONFIG_XF_COMMON_ENABLE_GNU
            #define XF_COMMON_ENABLE_GNU CONFIG_XF_COMMON_ENABLE_GNU
        #else
            #define XF_COMMON_ENABLE_GNU 0
        #endif
    #else
        #define XF_COMMON_ENABLE_GNU                1
    #endif
#endif
#ifndef XF_COMMON_ENABLE_BUILTIN
    #ifdef XF_KCONFIG_PRESENT
        #ifdef CONFIG_XF_COMMON_ENABLE_BUILTIN
            #define XF_COMMON_ENABLE_BUILTIN CONFIG_XF_COMMON_ENABLE_BUILTIN
        #else
            #define XF_COMMON_ENABLE_BUILTIN 0
        #endif
    #else
        #define XF_COMMON_ENABLE_BUILTIN            1
    #endif
#endif
#ifndef XF_COMMON_ENABLE_ERR_TO_NAME_LOOKUP
    #ifdef XF_KCONFIG_PRESENT
        #ifdef CONFIG_XF_COMMON_ENABLE_ERR_TO_NAME_LOOKUP
            #define XF_COMMON_ENABLE_ERR_TO_NAME_LOOKUP CONFIG_XF_COMMON_ENABLE_ERR_TO_NAME_LOOKUP
        #else
            #define XF_COMMON_ENABLE_ERR_TO_NAME_LOOKUP 0
        #endif
    #else
        #define XF_COMMON_ENABLE_ERR_TO_NAME_LOOKUP 1
    #endif
#endif

/* -------------------- components/dstruct ---------------------------------- */

/* -------------------- components/log -------------------------------------- */

#ifndef XF_LOG_ENABLE_CUSTOM_PORTING
    #ifdef CONFIG_XF_LOG_ENABLE_CUSTOM_PORTING
        #define XF_LOG_ENABLE_CUSTOM_PORTING CONFIG_XF_LOG_ENABLE_CUSTOM_PORTING
    #else
        #define XF_LOG_ENABLE_CUSTOM_PORTING        0
    #endif
#endif

/* 均可可独立开关 */
#ifndef XF_LOG_ENABLE_ERROR_LEVEL
    #ifdef XF_KCONFIG_PRESENT
        #ifdef CONFIG_XF_LOG_ENABLE_ERROR_LEVEL
            #define XF_LOG_ENABLE_ERROR_LEVEL CONFIG_XF_LOG_ENABLE_ERROR_LEVEL
        #else
            #define XF_LOG_ENABLE_ERROR_LEVEL 0
        #endif
    #else
        #define XF_LOG_ENABLE_ERROR_LEVEL           1
    #endif
#endif
#ifndef XF_LOG_ENABLE_WARN_LEVEL
    #ifdef XF_KCONFIG_PRESENT
        #ifdef CONFIG_XF_LOG_ENABLE_WARN_LEVEL
            #define XF_LOG_ENABLE_WARN_LEVEL CONFIG_XF_LOG_ENABLE_WARN_LEVEL
        #else
            #define XF_LOG_ENABLE_WARN_LEVEL 0
        #endif
    #else
        #define XF_LOG_ENABLE_WARN_LEVEL            1
    #endif
#endif
#ifndef XF_LOG_ENABLE_INFO_LEVEL
    #ifdef XF_KCONFIG_PRESENT
        #ifdef CONFIG_XF_LOG_ENABLE_INFO_LEVEL
            #define XF_LOG_ENABLE_INFO_LEVEL CONFIG_XF_LOG_ENABLE_INFO_LEVEL
        #else
            #define XF_LOG_ENABLE_INFO_LEVEL 0
        #endif
    #else
        #define XF_LOG_ENABLE_INFO_LEVEL            1
    #endif
#endif
#ifndef XF_LOG_ENABLE_DEBUG_LEVEL
    #ifdef XF_KCONFIG_PRESENT
        #ifdef CONFIG_XF_LOG_ENABLE_DEBUG_LEVEL
            #define XF_LOG_ENABLE_DEBUG_LEVEL CONFIG_XF_LOG_ENABLE_DEBUG_LEVEL
        #else
            #define XF_LOG_ENABLE_DEBUG_LEVEL 0
        #endif
    #else
        #define XF_LOG_ENABLE_DEBUG_LEVEL           1
    #endif
#endif
#ifndef XF_LOG_ENABLE_VERBOSE_LEVEL
    #ifdef XF_KCONFIG_PRESENT
        #ifdef CONFIG_XF_LOG_ENABLE_VERBOSE_LEVEL
            #define XF_LOG_ENABLE_VERBOSE_LEVEL CONFIG_XF_LOG_ENABLE_VERBOSE_LEVEL
        #else
            #define XF_LOG_ENABLE_VERBOSE_LEVEL 0
        #endif
    #else
        #define XF_LOG_ENABLE_VERBOSE_LEVEL         1
    #endif
#endif

/* -------------------- components/std -------------------------------------- */

/* XF_STD_STRING_* 只能二选一 */
#ifndef XF_STRING_ENABLE_BUILTIN
    #ifdef CONFIG_XF_STRING_ENABLE_BUILTIN
        #define XF_STRING_ENABLE_BUILTIN CONFIG_XF_STRING_ENABLE_BUILTIN
    #else
        #define XF_STRING_ENABLE_BUILTIN            0
    #endif
#endif
#ifndef XF_STRING_ENABLE_LIBC
    #ifdef XF_KCONFIG_PRESENT
        #ifdef CONFIG_XF_STRING_ENABLE_LIBC
            #define XF_STRING_ENABLE_LIBC CONFIG_XF_STRING_ENABLE_LIBC
        #else
            #define XF_STRING_ENABLE_LIBC 0
        #endif
    #else
        #define XF_STRING_ENABLE_LIBC               1
    #endif
#endif

/* -------------------- components/system ----------------------------------- */

/* -------------------- components/system/check ----------------------------- */

#ifndef XF_CHECK_ENABLE_CHECK
    #ifdef XF_KCONFIG_PRESENT
        #ifdef CONFIG_XF_CHECK_ENABLE_CHECK
            #define XF_CHECK_ENABLE_CHECK CONFIG_XF_CHECK_ENABLE_CHECK
        #else
            #define XF_CHECK_ENABLE_CHECK 0
        #endif
    #else
        #define XF_CHECK_ENABLE_CHECK               1
    #endif
#endif
#ifndef XF_CHECK_ENABLE_ASSERT
    #ifdef XF_KCONFIG_PRESENT
        #ifdef CONFIG_XF_CHECK_ENABLE_ASSERT
            #define XF_CHECK_ENABLE_ASSERT CONFIG_XF_CHECK_ENABLE_ASSERT
        #else
            #define XF_CHECK_ENABLE_ASSERT 0
        #endif
    #else
        #define XF_CHECK_ENABLE_ASSERT              1
    #endif
#endif
#ifndef XF_CHECK_ENABLE_ERROR_CHECK
    #ifdef XF_KCONFIG_PRESENT
        #ifdef CONFIG_XF_CHECK_ENABLE_ERROR_CHECK
            #define XF_CHECK_ENABLE_ERROR_CHECK CONFIG_XF_CHECK_ENABLE_ERROR_CHECK
        #else
            #define XF_CHECK_ENABLE_ERROR_CHECK 0
        #endif
    #else
        #define XF_CHECK_ENABLE_ERROR_CHECK         1
    #endif
#endif

/* -------------------- components/system/event ----------------------------- */

/* -------------------- components/system/ps -------------------------------- */

/* 内置消息队列中最大消息数量 */
#ifndef XF_PS_MSG_NUM_MAX
    #ifdef CONFIG_XF_PS_MSG_NUM_MAX
        #define XF_PS_MSG_NUM_MAX CONFIG_XF_PS_MSG_NUM_MAX
    #else
        #define XF_PS_MSG_NUM_MAX                   16
    #endif
#endif
/* 订阅者数量 */
#ifndef XF_PS_SUBSCRIBER_NUM_MAX
    #ifdef CONFIG_XF_PS_SUBSCRIBER_NUM_MAX
        #define XF_PS_SUBSCRIBER_NUM_MAX CONFIG_XF_PS_SUBSCRIBER_NUM_MAX
    #else
        #define XF_PS_SUBSCRIBER_NUM_MAX            16
    #endif
#endif

/* -------------------- components/system/safe ------------------------------ */

/* -------------------- components/system/stimer ---------------------------- */

/* 定时器数量 */
#ifndef XF_STIMER_NUM_MAX
    #ifdef CONFIG_XF_STIMER_NUM_MAX
        #define XF_STIMER_NUM_MAX CONFIG_XF_STIMER_NUM_MAX
    #else
        #define XF_STIMER_NUM_MAX                   16
    #endif
#endif
/* 没有定时器就绪时的延时时间，单位 tick */
#ifndef XF_STIMER_NO_READY_DELAY
    #ifdef CONFIG_XF_STIMER_NO_READY_DELAY
        #define XF_STIMER_NO_READY_DELAY CONFIG_XF_STIMER_NO_READY_DELAY
    #else
        #define XF_STIMER_NO_READY_DELAY            1000
    #endif
#endif

/* -------------------- components/system/task ------------------------------ */

/* XF_TASK_ENABLE_LC_* 二选一 */
#ifndef XF_TASK_ENABLE_LC_LABEL
    #ifdef XF_KCONFIG_PRESENT
        #ifdef CONFIG_XF_TASK_ENABLE_LC_LABEL
            #define XF_TASK_ENABLE_LC_LABEL CONFIG_XF_TASK_ENABLE_LC_LABEL
        #else
            #define XF_TASK_ENABLE_LC_LABEL 0
        #endif
    #else
        #define XF_TASK_ENABLE_LC_LABEL             1
    #endif
#endif
#ifndef XF_TASK_ENABLE_LC_SWITCH
    #ifdef CONFIG_XF_TASK_ENABLE_LC_SWITCH
        #define XF_TASK_ENABLE_LC_SWITCH CONFIG_XF_TASK_ENABLE_LC_SWITCH
    #else
        #define XF_TASK_ENABLE_LC_SWITCH            0
    #endif
#endif

/* 任务数量 */
#ifndef XF_TASK_NUM_MAX
    #ifdef CONFIG_XF_TASK_NUM_MAX
        #define XF_TASK_NUM_MAX CONFIG_XF_TASK_NUM_MAX
    #else
        #define XF_TASK_NUM_MAX                     16
    #endif
#endif
/* 任务嵌套深度，必须 >= 3 */
#ifndef XF_TASK_NEST_DEPTH_MAX
    #ifdef CONFIG_XF_TASK_NEST_DEPTH_MAX
        #define XF_TASK_NEST_DEPTH_MAX CONFIG_XF_TASK_NEST_DEPTH_MAX
    #else
        #define XF_TASK_NEST_DEPTH_MAX              6
    #endif
#endif
/* 任务事件消息缓存数量，至少为 1 。由于未测试，此处使用 4 */
#ifndef XF_TASK_EVENT_MSG_NUM_MAX
    #ifdef CONFIG_XF_TASK_EVENT_MSG_NUM_MAX
        #define XF_TASK_EVENT_MSG_NUM_MAX CONFIG_XF_TASK_EVENT_MSG_NUM_MAX
    #else
        #define XF_TASK_EVENT_MSG_NUM_MAX           4
    #endif
#endif

/* -------------------- components/system/tick ------------------------------ */

#ifndef XF_TICK_FREQ
    #ifdef CONFIG_XF_TICK_FREQ
        #define XF_TICK_FREQ CONFIG_XF_TICK_FREQ
    #else
        #define XF_TICK_FREQ                        1000
    #endif
#endif

/* -------------------- components/utils ------------------------------------ */

/* -------------------- examples -------------------------------------------- */

/* -------------------- Porting --------------------------------------------- */

/*  扩展配置存放 Kconfig 无法实现的功能，比如宏表达式。 */

#ifndef XF_ENABLE_PORTING
    #ifdef CONFIG_XF_ENABLE_PORTING
        #define XF_ENABLE_PORTING CONFIG_XF_ENABLE_PORTING
    #else
        #define XF_ENABLE_PORTING                   0
    #endif
#endif
#if XF_ENABLE_PORTING
#ifndef XF_PORTING_PATH
    #ifdef CONFIG_XF_PORTING_PATH
        #define XF_PORTING_PATH CONFIG_XF_PORTING_PATH
    #else
        #define XF_PORTING_PATH                     "xf_porting.h"
    #endif
#endif
#endif

/*----------------------------------
 * End of parsing xf_conf_template.h
 -----------------------------------*/

#if XF_ENABLE_PORTING
#include XF_PORTING_PATH
#endif

#endif /* __XF_CONF_INTERNAL_H__ */

/* *INDENT-ON* */
