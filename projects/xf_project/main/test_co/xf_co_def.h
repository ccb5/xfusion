/**
 * @file xf_co_def.h
 * @author catcatBlue (catcatblue@qq.com)
 * @brief
 * @version 1.0
 * @date 2025-05-18
 *
 * SPDX-FileCopyrightText: 2025 CompanyNameMagicTag
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#ifndef __XF_CO_DEF_H__
#define __XF_CO_DEF_H__

/* ==================== [Includes] ========================================== */

#include "xf_common.h"
#include "xf_dstruct.h"
#include "xf_log.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== [Defines] =========================================== */

#if 1 /* def XF_CO_LC_INCLUDE */
typedef void *xf_co_lc_t;
#define XF_CO_LC_INIT_VALUE     NULL
#define xf_co_lc_init(s)        s = XF_CO_LC_INIT_VALUE
#define xf_co_lc_resume(s)      do { if (s != XF_CO_LC_INIT_VALUE) { goto *s; } } while (0)
#define xf_co_lc_set(s)         do { \
                                    XCAT2(XF_CO_LC_LABEL, __LINE__): \
                                    (s) = &&XCAT2(XF_CO_LC_LABEL, __LINE__); \
                                } while (0)
#define xf_co_lc_end(s)
#else
typedef uint16_t xf_co_lc_t;
#define XF_CO_LC_INIT_VALUE     0
#define xf_co_lc_init(s)        s = XF_CO_LC_INIT_VALUE;
#define xf_co_lc_resume(s)      switch(s) { /* fall-through */ /* FALLTHRU */ case XF_CO_LC_INIT_VALUE:
#define xf_co_lc_set(s)         s = __LINE__; /* fall-through */ /* FALLTHRU */ case __LINE__:
#define xf_co_lc_end(s)         }
#endif

/**
 * @brief 无效状态。
 * - 协程未初始化或已终止。
 */
#define XF_CO_STATE_INACTIVE          (0x00)
#define XF_CO_STATE_TERMINATED        XF_CO_STATE_INACTIVE

/**
 * @brief 就绪状态。
 *
 * 如果一个协程没有被挂起 (SUSPENDED)，那么它就是就绪 (READY) 状态。
 *
 * @details
 * - 没有调度器的情况：
 *   - 区别于线程，对于协程，即使协程主动 delay 或等待 event ，也需要进入协程内查询状态，
 *     因此协程大部分情况下都处于就绪 (READY) 或等待 (WAIT) 状态，
 *     而不是阻塞 (BLOCKED) 状态，以便进入协程查询能否执行。
 * - 有调度器的情况：
 *   - 在调度器中可以主动 **不** 执行未到唤醒时刻的协程。
 *   - 如果在 @ref xf_co_t 内增加事件查询回调，那么可实现
 *     “调度器通过调用回调，查询协程等待的事件是否发生，从而决定是否实际执行协程”
 *     的目的，但如果需要使用回调函数，那么就违背了 xf_co 简易使用的初衷。
 */
#define XF_CO_STATE_READY           (0x01)
#define XF_CO_STATE_WAITING         XF_CO_STATE_READY

/**
 * @brief 运行状态。
 */
#define XF_CO_STATE_RUNNING         (0x02)

/**
 * @brief 阻塞状态。
 * - 协程被挂起 (SUSPENDED) 时，会进入阻塞 (BLOCKED) 状态。
 */
#define XF_CO_STATE_BLOCKED         (0x03)
#define XF_CO_STATE_SUSPENDED       XF_CO_STATE_BLOCKED

/* ==================== [Typedefs] ========================================== */

/**
 * @brief 无栈协程时间戳。
 */
typedef uint32_t /* uint64_t */ xf_co_timestamp_t;

/**
 * @brief 协程类型标识。
 */
typedef uint8_t xf_co_type_t;
enum _xf_co_type_t {
    XF_CO_TYPE_UNKNOWN = 0,                 /*!< 未知类型 */
    XF_CO_TYPE_NCTX,                        /*!< 无用户上下文的协程 @ref xf_co_nctx_t */
    XF_CO_TYPE_GEN,                         /*!< 通用协程 @ref xf_co_gen_t */
    XF_CO_TYPE_SCHEDULER,                   /*!< 调度器协程 @ref xf_co_sched_t */
    XF_CO_TYPE_USER,                        /*!< 自定义协程类型，起始值 */
    XF_CO_TYPE_MAX,                         /*!< （不含）协程类型最大值, 16 */
};

/**
 * @brief 协程优先级。（只使用 4 位）
 * 不是实时调度，只是用于插入调度链时排序。
 */
typedef uint8_t xf_co_priority_t;
enum _xf_co_priority_t {
    XF_CO_PRIORITY_NONE          =  0,         /*!< No priority (not initialized). */
    XF_CO_PRIORITY_IDLE          =  1,         /*!< Reserved for Idle thread. */
    XF_CO_PRIORITY_LOW           =  4,         /*!< low */
    XF_CO_PRIORITY_LOW1          =  4 + 1,     /*!< low + 1 */
    XF_CO_PRIORITY_LOW2          =  4 + 2,     /*!< low + 2 */
    XF_CO_PRIORITY_LOW3          =  4 + 3,     /*!< low + 3 */
    XF_CO_PRIORITY_BELOWNORMAL   =  8,         /*!< below normal */
    XF_CO_PRIORITY_BELOWNORMAL1  =  8 + 1,     /*!< below normal + 1 */
    XF_CO_PRIORITY_BELOWNORMAL2  =  8 + 2,     /*!< below normal + 2 */
    XF_CO_PRIORITY_BELOWNORMAL3  =  8 + 3,     /*!< below normal + 3 */
    XF_CO_PRIORITY_NORMAL        = 12,         /*!< normal */
    XF_CO_PRIORITY_NORMAL1       = 12 + 1,     /*!< normal + 1 */
    XF_CO_PRIORITY_NORMAL2       = 12 + 2,     /*!< normal + 2 */
    XF_CO_PRIORITY_NORMAL3       = 12 + 3,     /*!< normal + 3 */
    XF_CO_PRIORITY_ABOVENORMAL   = 16,         /*!< above normal */
    XF_CO_PRIORITY_ABOVENORMAL1  = 16 + 1,     /*!< above normal + 1 */
    XF_CO_PRIORITY_ABOVENORMAL2  = 16 + 2,     /*!< above normal + 2 */
    XF_CO_PRIORITY_ABOVENORMAL3  = 16 + 3,     /*!< above normal + 3 */
    XF_CO_PRIORITY_HIGH          = 20,         /*!< high */
    XF_CO_PRIORITY_HIGH1         = 20 + 1,     /*!< high + 1 */
    XF_CO_PRIORITY_HIGH2         = 20 + 2,     /*!< high + 2 */
    XF_CO_PRIORITY_HIGH3         = 20 + 3,     /*!< high + 3 */
    XF_CO_PRIORITY_REALTIME      = 24,         /*!< realtime */
    XF_CO_PRIORITY_REALTIME1     = 24 + 1,     /*!< realtime + 1 */
    XF_CO_PRIORITY_REALTIME2     = 24 + 2,     /*!< realtime + 2 */
    XF_CO_PRIORITY_REALTIME3     = 24 + 3,     /*!< realtime + 3 */
    XF_CO_PRIORITY_ISR           = 28,         /*!< 预留给 ISR 延迟线程 */
    XF_CO_PRIORITY_SCHEDULER     = 30,         /*!< 调度器优先级 */
    XF_CO_PRIORITY_ERROR         = 31,         /*!< 系统无法确定优先级或非法优先级 */
};

typedef struct xf_co_sched xf_co_sched_t;

typedef struct xf_co_attr {
    xf_co_type_t            type;                   /*!< 协程类型 */
    xf_co_priority_t        priority;               /*!< 优先级 */
    xf_co_sched_t          *co_sched;               /*!< 所属调度器 */
#if CONFIG_XF_CO_ENABLE_USER_DATA
    void                   *user_data;
#endif
} xf_co_attr_t;

/**
 * @brief 无栈协程状态。
 *
 * xf_co_func_t 返回值可能是：
 *      - XF_CO_TERMINATED
 *      - XF_CO_WAITING
 *      - XF_CO_SUSPENDED
 * 不可能返回：
 *      - XF_CO_RUNNING
 */
typedef uint8_t xf_co_state_t;
enum _xf_co_state_t {
    XF_CO_INACTIVE      = XF_CO_STATE_INACTIVE,
    XF_CO_TERMINATED    = XF_CO_INACTIVE,
    XF_CO_READY         = XF_CO_STATE_READY,
    XF_CO_WAITING       = XF_CO_READY,
    XF_CO_RUNNING       = XF_CO_STATE_RUNNING,
    XF_CO_SUSPENDED     = XF_CO_STATE_SUSPENDED,
    XF_CO_STATE_MAX,
};
typedef xf_co_state_t xf_co_async_t; /*!< 用于标识无栈协程函数 */

typedef uint8_t xf_co_sched_state_t;

/**
 * @brief 无栈协程基类预声明。
 */
typedef struct xf_co xf_co_t;

/**
 * @brief 无栈协程函数。
 *
 * @param me    协程指针。
 * @param e     事件。通常为事件数据。事件类型可用 @ref xf_co_t.eid 标识。
 * @return xf_co_async_t 协程状态。
 *      - XF_CO_TERMINATED      已终止
 *      - XF_CO_WAITING         等待下一次执行
 *      // - XF_CO_SUSPENDED       此协程已挂起
 */
typedef xf_co_async_t (*xf_co_func_t)(xf_co_t *const me, void *e);

/**
 * @brief 无栈协程上下文（系统上下文）基类。
 */
struct xf_co {
    /**
     * @brief Local Continuations（本地延续，当前代码的执行位置）。
     */
    volatile xf_co_lc_t     lc;

    /**
     * @brief 状态标志 (flags)。
     *
     * @details
     * 1. B0 ~ B1: 协程状态 (state).
     *
     *    类型见 @ref xf_co_state_t. 四种状态均可能在此处出现。
     *
     * 2. B2: 请求终止位 (terminate_bit)。
     *
     *    置 1 时请求终止，成功终止时 flags[0..2] 为 0x00.
     *    其中，成功终止指：
     *    协程接受了终止信号，并从 xf_co_end() 分支退出。
     *    此时通常意味着回收了资源。
     *
     * 3. B3: 等待位 (await).
     *
     *    正在等待子协程完成。为 1 时正在等待。
     *
     * 3. B4 ~ B7: 保留位 (reserved).
     *
     *    暂未使用。用法可参考 "Thread Flags"。
     */
    volatile uint8_t        flags;          /*!< 状态标志 */
#define XF_CO_FLAGS_STATE_S         (0)     /*!< 起始位 */
#define XF_CO_FLAGS_STATE_W         (2)     /*!< 宽度 */
#define XF_CO_FLAGS_STATE_MASK      (0x03)  /*!< 状态掩码 (B0 ~ B1) */
#define XF_CO_FLAGS_TERMINATE_BIT   (0x02)  /*!< 请求终止位 (B2) */
#define XF_CO_FLAGS_AWAIT_BIT       (0x02)  /*!< 等待位 (B3) */
#define XF_CO_FLAGS_RESERVED_S      (4)     /*!< 起始位 */
#define XF_CO_FLAGS_RESERVED_W      (4)     /*!< 宽度 */
#define XF_CO_FLAGS_RESERVED_MASK   (0xF0)  /*!< 保留位掩码 (B4 ~ B7) */

    /**
     * @brief 属性 (attributes)。
     *
     * @details
     * 1. B0 ~ B3: 优先级 (priority).
     *
     *    范围 [0, 31]. 不是实时调度，只是用来插入调度链时排序。
     *    数字越大，优先级越高。见 @ref xf_co_priority_t.
     *    优先级还用于标记此协程是否已初始化。见 @ref xf_co_await .
     *    - XF_CO_PRIORITY_NONE 未初始化。
     *    - (OTHER) 已初始化。
     *
     * 2. B4 ~ B7: 协程类型 (type).
     *
     *    范围 [0, 31]. 用于标识扩展类型，见 @ref xf_co_type_t.
     *
     * 3. TODO: 协程特性 (feature).
     *
     *    是否支持：调度、平行嵌套、协程标志、订阅标志、上次切出的时间
     */
    uint8_t                 attrs;
#define XF_CO_ATTRS_PRIO_S          (0)     /*!< 起始位 */
#define XF_CO_ATTRS_PRIO_W          (5)     /*!< 宽度 */
#define XF_CO_ATTRS_PRIO_MASK       (0x1F)  /*!< 优先级掩码 (B0 ~ B5) */
#define XF_CO_ATTRS_TYPE_S          (5)     /*!< 起始位 */
#define XF_CO_ATTRS_TYPE_W          (3)     /*!< 宽度 */
#define XF_CO_ATTRS_TYPE_MASK       (0xE0)  /*!< 类型掩码 (B0 ~ B5) */

    /**
     * @brief 用户信号。
     *
     * 大小 uint16_t，用法不做强转规定。
     */
    volatile uint16_t       user_signal;

    xf_co_func_t            func;           /*!< 协程执行函数 */
    volatile xf_co_timestamp_t  ts_wakeup;  /*!< 唤醒时间戳 */
    volatile xf_co_timestamp_t  ts_timeout; /*!< 超时时间戳（关系是 wakeup->wakeup->...->timeout ） */
#define XF_CO_MIN_INTERVAL  (1000)          /*!< （调度器）最小轮询时间间隔，单位：时基 tick */

    xf_list_node_t          node_w;         /*!< 平行协程节点（广度） */
#if XF_TODO
    xf_list_node_t          node_d;         /*!< 调用层级节点（深度） */
#endif
#if XF_TODO
    xf_co_timestamp_t       ts_yield;       /*!< 本次进入时，上次切出的时间 */
#endif
#if CONFIG_XF_CO_ENABLE_USER_DATA
    void                   *user_data;      /*!< 用户数据 */
#endif
    /* TODO 扩展功能虚表 */
};

// struct xf_co {
//     /**
//      * @brief Local Continuations（本地延续，当前代码的执行位置）。
//      */
//     volatile xf_co_lc_t     lc;
//     xf_co_func_t            func;           /*!< 协程执行函数 */
//     void                   *user_data;      /*!< 用户数据 */
// };

/**
 * @brief 调度器的协程类型。一般不要使用。
 */
struct xf_co_sched {
    xf_co_t                 base;       /*!< 必须第一个，名字必须为 base */
    xf_list_head_t          head;       /*!< 调度器协程链表头 */
    uint8_t                 count;      /*!< 此调度器下协程数量 */
};

/**
 * @brief 无用户上下文的协程类型。
 */
typedef struct xf_co_no_context {
    xf_co_t                 base;           /*!< 必须第一个，名字必须为 base */
} xf_co_nctx_t;

/**
 * @brief 通用协程类型。
 */
typedef struct xf_co_general {
    xf_co_t                 base;           /*!< 必须第一个，名字必须为 base */
    uintptr_t
    uip;             /*!< 无符号整型指针类型 (U Int Ptr)，可用作用户数据类型标识 */
    void                   *vd;             /*!< 空指针类型数据 (Void Data)，可用作用户数据 */
} xf_co_gen_t;

/* TODO 原子操作 */
typedef struct xf_co_mutex {
    volatile uint8_t        flag;
#define XF_CO_MUTEX_LOCKED              (1)
#define XF_CO_MUTEX_UNLOCKED            (0)
#if CONFIG_XF_CO_ENABLE_MUTEX_OWNER
    xf_co_t                *owner;
#   if CONFIG_XF_CO_ENABLE_MUTEX_RECURSIVE
    /* TODO 递归互斥锁 */
    volatile uint8_t        count;
#   endif
#endif
} xf_co_mutex_t;

/* TODO 原子操作 */
typedef struct xf_co_semaphore {
    volatile uint8_t        count;
    uint8_t                 count_max;
} xf_co_semaphore_t;

/* ==================== [Global Prototypes] ================================= */

/* ==================== [Macros] ============================================ */

#define xf_co_cast(_co)                 ((xf_co_t *const)(_co))
#define xf_co_func_cast(_co_func)       ((xf_co_func_t)(_co_func))

#define xf_co_set_flags_state(_co, _value) \
                                        BITSn_SET(xf_co_cast(_co)->flags, \
                                                  XF_CO_FLAGS_STATE_W, XF_CO_FLAGS_STATE_S, \
                                                  (_value))

#define xf_co_get_flags_state(_co)      BITSn_GET_RSH(xf_co_cast(_co)->flags, \
                                            XF_CO_FLAGS_STATE_W, XF_CO_FLAGS_STATE_S)

#define xf_co_set_flags_terminate_bit(_co, _value) \
                                        BIT_SET(xf_co_cast(_co)->flags, \
                                                XF_CO_FLAGS_TERMINATE_BIT, \
                                                (_value))

#define xf_co_get_flags_terminate_bit(_co) \
                                        BIT_GET(xf_co_cast(_co)->flags, \
                                                XF_CO_FLAGS_TERMINATE_BIT)

#define xf_co_set_flags_await_bit(_co, _value) \
                                        BIT_SET(xf_co_cast(_co)->flags, \
                                                XF_CO_FLAGS_AWAIT_BIT, \
                                                (_value))

#define xf_co_get_flags_await_bit(_co) \
                                        BIT_GET(xf_co_cast(_co)->flags, \
                                                XF_CO_FLAGS_AWAIT_BIT)

#define xf_co_set_attrs_priority(_co, _priority) \
                                        BITSn_SET(xf_co_cast(_co)->attrs, \
                                                  XF_CO_ATTRS_PRIO_W, XF_CO_ATTRS_PRIO_S, \
                                                  (_priority)) \

#define xf_co_get_attrs_priority(_co)   BITSn_GET_RSH(xf_co_cast(_co)->attrs, \
                                            XF_CO_ATTRS_PRIO_W, XF_CO_ATTRS_PRIO_S)

#define xf_co_set_attrs_type(_co, _type) \
                                        BITSn_SET(xf_co_cast(_co)->attrs, \
                                                  XF_CO_ATTRS_TYPE_W, XF_CO_ATTRS_TYPE_S, \
                                                  (_type)) \

#define xf_co_get_attrs_type(_co)       BITSn_GET_RSH(xf_co_cast(_co)->attrs, \
                                            XF_CO_ATTRS_TYPE_W, XF_CO_ATTRS_TYPE_S)

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __XF_CO_DEF_H__ */
