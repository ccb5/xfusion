/**
 * @file xf_conf.h
 *
 * Configuration file v1.0.0 for xtiny vx.x.x
 *
 */

/*
 * Copy this file as `xf_conf.h`
 * 1. simply next to `xtiny` folder
 * 2. or to any other place and
 *    - define `XF_CONF_INCLUDE_SIMPLE`;
 *    - add the path as an include path.
 */

/* clang-format off */
#if 1 /* Set this to "1" to enable content */

#ifndef __XF_CONF_H__
#define __XF_CONF_H__

/* If you need to include anything here, do it inside the `__ASSEMBLY__` guard */
#if  0
#include "my_include.h"
#endif

/*  组件划分单位：内含 `CMakeLists.txt` 视为独立组件。 */

/* -------------------- components ------------------------------------------ */

/* -------------------- components/algo ------------------------------------- */

/* -------------------- components/common ----------------------------------- */

#define XF_COMMON_ENABLE_64BITS             1
#define XF_COMMON_ENABLE_GNU                1
#define XF_COMMON_ENABLE_BUILTIN            1
#define XF_COMMON_ENABLE_ERR_TO_NAME_LOOKUP 1

/* -------------------- components/dstruct ---------------------------------- */

/* -------------------- components/log -------------------------------------- */

#define XF_LOG_ENABLE_CUSTOM_PORTING        0

/* 均可可独立开关 */
#define XF_LOG_ENABLE_ERROR_LEVEL           1
#define XF_LOG_ENABLE_WARN_LEVEL            1
#define XF_LOG_ENABLE_INFO_LEVEL            1
#define XF_LOG_ENABLE_DEBUG_LEVEL           1
#define XF_LOG_ENABLE_VERBOSE_LEVEL         1

/* -------------------- components/std -------------------------------------- */

/* XF_STD_STRING_* 只能二选一 */
#define XF_STRING_ENABLE_BUILTIN            0
#define XF_STRING_ENABLE_LIBC               1

/* -------------------- components/system ----------------------------------- */

/* -------------------- components/system/check ----------------------------- */

#define XF_CHECK_ENABLE_CHECK               1
#define XF_CHECK_ENABLE_ASSERT              1
#define XF_CHECK_ENABLE_ERROR_CHECK         1

/* -------------------- components/system/event ----------------------------- */

/* -------------------- components/system/ps -------------------------------- */

/* 内置消息队列中最大消息数量 */
#define XF_PS_MSG_NUM_MAX                   16
/* 订阅者数量 */
#define XF_PS_SUBSCRIBER_NUM_MAX            16

/* -------------------- components/system/safe ------------------------------ */

/* -------------------- components/system/stimer ---------------------------- */

/* 定时器数量 */
#define XF_STIMER_NUM_MAX                   16
/* 没有定时器就绪时的延时时间，单位 tick */
#define XF_STIMER_NO_READY_DELAY            1000

/* -------------------- components/system/task ------------------------------ */

/* XF_TASK_ENABLE_LC_* 二选一 */
#define XF_TASK_ENABLE_LC_LABEL             1
#define XF_TASK_ENABLE_LC_SWITCH            0

/* 任务数量 */
#define XF_TASK_NUM_MAX                     16
/* 任务嵌套深度，必须 >= 3 */
#define XF_TASK_NEST_DEPTH_MAX              6
/* 任务事件消息缓存数量，至少为 1 。由于未测试，此处使用 4 */
#define XF_TASK_EVENT_MSG_NUM_MAX           4

/* -------------------- components/system/tick ------------------------------ */

#define XF_TICK_FREQ                        1000

/* -------------------- components/utils ------------------------------------ */

/* -------------------- examples -------------------------------------------- */

/* -------------------- Porting --------------------------------------------- */

/*  扩展配置存放 Kconfig 无法实现的功能，比如宏表达式。 */

#define XF_ENABLE_PORTING                   0
#if XF_ENABLE_PORTING
#define XF_PORTING_PATH                     "xf_porting.h"
#endif

/* --END OF __XF_CONF_H__-- */

#endif /* __XF_CONF_H__ */

#endif /* End of "Content enable" */
