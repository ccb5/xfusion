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
    #if !defined(XF_CONF_H) && !defined(XF_CONF_SUPPRESS_DEFINE_CHECK)
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

#ifndef XF_ENABLE_ALGO
    #ifdef XF_KCONFIG_PRESENT
        #ifdef CONFIG_XF_ENABLE_ALGO
            #define XF_ENABLE_ALGO CONFIG_XF_ENABLE_ALGO
        #else
            #define XF_ENABLE_ALGO 0
        #endif
    #else
        #define XF_ENABLE_ALGO                      1
    #endif
#endif

/* -------------------- components/common ----------------------------------- */

#ifndef XF_ENABLE_COMMON
    #ifdef XF_KCONFIG_PRESENT
        #ifdef CONFIG_XF_ENABLE_COMMON
            #define XF_ENABLE_COMMON CONFIG_XF_ENABLE_COMMON
        #else
            #define XF_ENABLE_COMMON 0
        #endif
    #else
        #define XF_ENABLE_COMMON                    1
    #endif
#endif

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

#ifndef XF_ENABLE_DSTRUCT
    #ifdef XF_KCONFIG_PRESENT
        #ifdef CONFIG_XF_ENABLE_DSTRUCT
            #define XF_ENABLE_DSTRUCT CONFIG_XF_ENABLE_DSTRUCT
        #else
            #define XF_ENABLE_DSTRUCT 0
        #endif
    #else
        #define XF_ENABLE_DSTRUCT                   1
    #endif
#endif

/* 目前禁止开启 XF_DEQUE_ENABLE_ZERO_LENGTH_ARRAYS 和关闭 XF_DEQUE_ENABLE_BUFFER_POINTER */
#ifndef XF_DEQUE_ENABLE_ZERO_LENGTH_ARRAYS
    #ifdef CONFIG_XF_DEQUE_ENABLE_ZERO_LENGTH_ARRAYS
        #define XF_DEQUE_ENABLE_ZERO_LENGTH_ARRAYS CONFIG_XF_DEQUE_ENABLE_ZERO_LENGTH_ARRAYS
    #else
        #define XF_DEQUE_ENABLE_ZERO_LENGTH_ARRAYS  0
    #endif
#endif
#ifndef XF_DEQUE_ENABLE_BUFFER_POINTER
    #ifdef XF_KCONFIG_PRESENT
        #ifdef CONFIG_XF_DEQUE_ENABLE_BUFFER_POINTER
            #define XF_DEQUE_ENABLE_BUFFER_POINTER CONFIG_XF_DEQUE_ENABLE_BUFFER_POINTER
        #else
            #define XF_DEQUE_ENABLE_BUFFER_POINTER 0
        #endif
    #else
        #define XF_DEQUE_ENABLE_BUFFER_POINTER      1
    #endif
#endif

/* -------------------- components/log -------------------------------------- */

#ifndef XF_ENABLE_LOG
    #ifdef XF_KCONFIG_PRESENT
        #ifdef CONFIG_XF_ENABLE_LOG
            #define XF_ENABLE_LOG CONFIG_XF_ENABLE_LOG
        #else
            #define XF_ENABLE_LOG 0
        #endif
    #else
        #define XF_ENABLE_LOG                       1
    #endif
#endif

/* 均可可独立开关 */
#ifndef XF_LOG_ENABLE_ERROR
    #ifdef XF_KCONFIG_PRESENT
        #ifdef CONFIG_XF_LOG_ENABLE_ERROR
            #define XF_LOG_ENABLE_ERROR CONFIG_XF_LOG_ENABLE_ERROR
        #else
            #define XF_LOG_ENABLE_ERROR 0
        #endif
    #else
        #define XF_LOG_ENABLE_ERROR                 1
    #endif
#endif
#ifndef XF_LOG_ENABLE_WARN
    #ifdef XF_KCONFIG_PRESENT
        #ifdef CONFIG_XF_LOG_ENABLE_WARN
            #define XF_LOG_ENABLE_WARN CONFIG_XF_LOG_ENABLE_WARN
        #else
            #define XF_LOG_ENABLE_WARN 0
        #endif
    #else
        #define XF_LOG_ENABLE_WARN                  1
    #endif
#endif
#ifndef XF_LOG_ENABLE_INFO
    #ifdef XF_KCONFIG_PRESENT
        #ifdef CONFIG_XF_LOG_ENABLE_INFO
            #define XF_LOG_ENABLE_INFO CONFIG_XF_LOG_ENABLE_INFO
        #else
            #define XF_LOG_ENABLE_INFO 0
        #endif
    #else
        #define XF_LOG_ENABLE_INFO                  1
    #endif
#endif
#ifndef XF_LOG_ENABLE_DEBUG
    #ifdef XF_KCONFIG_PRESENT
        #ifdef CONFIG_XF_LOG_ENABLE_DEBUG
            #define XF_LOG_ENABLE_DEBUG CONFIG_XF_LOG_ENABLE_DEBUG
        #else
            #define XF_LOG_ENABLE_DEBUG 0
        #endif
    #else
        #define XF_LOG_ENABLE_DEBUG                 1
    #endif
#endif
#ifndef XF_LOG_ENABLE_VERBOSE
    #ifdef XF_KCONFIG_PRESENT
        #ifdef CONFIG_XF_LOG_ENABLE_VERBOSE
            #define XF_LOG_ENABLE_VERBOSE CONFIG_XF_LOG_ENABLE_VERBOSE
        #else
            #define XF_LOG_ENABLE_VERBOSE 0
        #endif
    #else
        #define XF_LOG_ENABLE_VERBOSE               1
    #endif
#endif

/* -------------------- components/std -------------------------------------- */

#ifndef XF_ENABLE_STD
    #ifdef XF_KCONFIG_PRESENT
        #ifdef CONFIG_XF_ENABLE_STD
            #define XF_ENABLE_STD CONFIG_XF_ENABLE_STD
        #else
            #define XF_ENABLE_STD 0
        #endif
    #else
        #define XF_ENABLE_STD                       1
    #endif
#endif

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

#ifndef XF_ENABLE_SYSTEM
    #ifdef XF_KCONFIG_PRESENT
        #ifdef CONFIG_XF_ENABLE_SYSTEM
            #define XF_ENABLE_SYSTEM CONFIG_XF_ENABLE_SYSTEM
        #else
            #define XF_ENABLE_SYSTEM 0
        #endif
    #else
        #define XF_ENABLE_SYSTEM                    1
    #endif
#endif

/* -------------------- components/system/check ----------------------------- */

#ifndef XF_ENABLE_CHECK
    #ifdef XF_KCONFIG_PRESENT
        #ifdef CONFIG_XF_ENABLE_CHECK
            #define XF_ENABLE_CHECK CONFIG_XF_ENABLE_CHECK
        #else
            #define XF_ENABLE_CHECK 0
        #endif
    #else
        #define XF_ENABLE_CHECK                     1
    #endif
#endif

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

#ifndef XF_ENABLE_EVENT
    #ifdef XF_KCONFIG_PRESENT
        #ifdef CONFIG_XF_ENABLE_EVENT
            #define XF_ENABLE_EVENT CONFIG_XF_ENABLE_EVENT
        #else
            #define XF_ENABLE_EVENT 0
        #endif
    #else
        #define XF_ENABLE_EVENT                     1
    #endif
#endif

/* -------------------- components/system/ps -------------------------------- */

#ifndef XF_ENABLE_PS
    #ifdef XF_KCONFIG_PRESENT
        #ifdef CONFIG_XF_ENABLE_PS
            #define XF_ENABLE_PS CONFIG_XF_ENABLE_PS
        #else
            #define XF_ENABLE_PS 0
        #endif
    #else
        #define XF_ENABLE_PS                        1
    #endif
#endif

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

#ifndef XF_ENABLE_SAFE
    #ifdef XF_KCONFIG_PRESENT
        #ifdef CONFIG_XF_ENABLE_SAFE
            #define XF_ENABLE_SAFE CONFIG_XF_ENABLE_SAFE
        #else
            #define XF_ENABLE_SAFE 0
        #endif
    #else
        #define XF_ENABLE_SAFE                      1
    #endif
#endif

/* !!! For configuration details, please refer to xf_conf_extended. !!! */

/* -------------------- components/system/stimer ---------------------------- */

#ifndef XF_ENABLE_STIMER
    #ifdef XF_KCONFIG_PRESENT
        #ifdef CONFIG_XF_ENABLE_STIMER
            #define XF_ENABLE_STIMER CONFIG_XF_ENABLE_STIMER
        #else
            #define XF_ENABLE_STIMER 0
        #endif
    #else
        #define XF_ENABLE_STIMER                    1
    #endif
#endif
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

#ifndef XF_ENABLE_TASK
    #ifdef XF_KCONFIG_PRESENT
        #ifdef CONFIG_XF_ENABLE_TASK
            #define XF_ENABLE_TASK CONFIG_XF_ENABLE_TASK
        #else
            #define XF_ENABLE_TASK 0
        #endif
    #else
        #define XF_ENABLE_TASK                      1
    #endif
#endif

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

#ifndef XF_ENABLE_TICK
    #ifdef XF_KCONFIG_PRESENT
        #ifdef CONFIG_XF_ENABLE_TICK
            #define XF_ENABLE_TICK CONFIG_XF_ENABLE_TICK
        #else
            #define XF_ENABLE_TICK 0
        #endif
    #else
        #define XF_ENABLE_TICK                      1
    #endif
#endif

#ifndef XF_TICK_FREQ
    #ifdef CONFIG_XF_TICK_FREQ
        #define XF_TICK_FREQ CONFIG_XF_TICK_FREQ
    #else
        #define XF_TICK_FREQ                        1000
    #endif
#endif

/* -------------------- components/utils ----------------------------------- */

#ifndef XF_ENABLE_UTILS
    #ifdef XF_KCONFIG_PRESENT
        #ifdef CONFIG_XF_ENABLE_UTILS
            #define XF_ENABLE_UTILS CONFIG_XF_ENABLE_UTILS
        #else
            #define XF_ENABLE_UTILS 0
        #endif
    #else
        #define XF_ENABLE_UTILS                     1
    #endif
#endif

/* -------------------- Extended Configuration ------------------------------ */

/*  扩展配置存放 Kconfig 无法实现的功能，比如宏表达式。 */

#ifndef XF_ENABLE_EXTENDED
    #ifdef CONFIG_XF_ENABLE_EXTENDED
        #define XF_ENABLE_EXTENDED CONFIG_XF_ENABLE_EXTENDED
    #else
        #define XF_ENABLE_EXTENDED                  0
    #endif
#endif
#if XF_ENABLE_EXTENDED
#ifndef XF_CONF_EXTENDED_PATH
    #ifdef CONFIG_XF_CONF_EXTENDED_PATH
        #define XF_CONF_EXTENDED_PATH CONFIG_XF_CONF_EXTENDED_PATH
    #else
        #define XF_CONF_EXTENDED_PATH               "xf_conf_extended.h"
    #endif
#endif
#endif /* XF_ENABLE_EXTENDED */

/*----------------------------------
 * End of parsing xf_conf_template.h
 -----------------------------------*/

#if XF_ENABLE_EXTENDED
#include XF_CONF_EXTENDED_PATH
#endif

#endif /* __XF_CONF_INTERNAL_H__ */

/* *INDENT-ON* */
