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

#include "xf_event.h"
#include "xf_ps.h"
#include "xf_stimer.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== [Defines] =========================================== */

/* 协程数量 */
/* 
    ID 为 XF_CO_NUM_MAX - 1 的协程默认被 xf_co_top 占用
 */
#define XF_CO_NUM_MAX                   32U
#define XF_CO_INTERNAL_NUM_MAX          32U
/*
    maximum depth of state nesting in a coroutine (including the top level),
    must be >= 3
 */
#define XF_CO_NEST_DEPTH_MAX            (10)

#define CONFIG_XF_CO_ENABLE_USER_DATA   1

#if XF_CO_INTERNAL_NUM_MAX > XF_CO_NUM_MAX
#   error ""
#endif

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

/* ==================== [Typedefs] ========================================== */

/**
 * @brief 协程类型标识。
 */
typedef uint8_t xf_co_type_t;
enum _xf_co_type_t {
    XF_CO_TYPE_UNKNOWN = 0x00,              /*!< 未知类型，视为无用户上下文 @ref xf_co_nctx_t */
    XF_CO_TYPE_GEN,                         /*!< 通用协程 @ref xf_co_gen_t */
    XF_CO_TYPE_TOP,                         /*!< 顶层协程 */
    XF_CO_TYPE_USER,                        /*!< 自定义协程类型，起始值 */
    XF_CO_TYPE_MAX,                         /*!< （不含）协程类型最大值, 16 */
};

/**
 * @brief 协程唯一 ID ，也是优先级。
 * ID 越大，协程的优先级越高。
 * 处理事件时，优先级高的协程会先处理事件。
 */
typedef uint8_t xf_co_id_t;

typedef struct xf_co_attr {
    xf_co_type_t            type;                   /*!< 协程类型 */
    xf_co_id_t              id;                     /*!< id */
} xf_co_attr_t;

/**
 * @brief 无栈协程状态。
 *
 * xf_co_func_t 返回值可能是：
 *      - XF_CO_DEAD
 *      - XF_CO_NORMAL
 *      - XF_CO_SUSPENDED
 * 无栈协程不可能返回 XF_CO_RUNNING.
 */
typedef uint8_t xf_co_state_t;
enum _xf_co_state_t {
    _XF_CO_DEAD       = 0x00U,              /*!< 已结束 */
    _XF_CO_NORMAL,                          /*!< 正常 */
    _XF_CO_RUNNING,                         /*!< 正在运行 */
    _XF_CO_SUSPENDED,                       /*!< 已挂起 */
    _XF_CO_STATE_MAX,                       /*!< （不含）协程状态最大值 */
};
#define XF_CO_DEAD                          ((xf_co_state_t)_XF_CO_DEAD     )
#define XF_CO_NORMAL                        ((xf_co_state_t)_XF_CO_NORMAL   )
#define XF_CO_RUNNING                       ((xf_co_state_t)_XF_CO_RUNNING  )
#define XF_CO_SUSPENDED                     ((xf_co_state_t)_XF_CO_SUSPENDED)
#define XF_CO_STATE_MAX                     ((xf_co_state_t)_XF_CO_STATE_MAX)

typedef xf_co_state_t xf_co_async_t; /*!< 用于标识无栈协程函数 */

typedef volatile uint16_t xf_co_flags_t;

/**
 * @brief 无栈协程基类预声明。
 */
typedef struct xf_co xf_co_t;

/**
 * @brief 无栈协程函数。
 *
 * @param me    协程指针。实现函数时必须使用此名称。
 * @param arg   外部传入参数。实现函数时必须使用此名称。
 * @return xf_co_state_t 协程状态。
 *      - XF_CO_DEAD            已终止
 *      - XF_CO_NORMAL          正常
 *      - XF_CO_RUNNING         运行中
 *      - XF_CO_SUSPENDED       此协程已挂起
 */
typedef xf_co_state_t (*xf_co_func_t)(xf_co_t *const me, void *arg);

typedef union xf_co_temp {
    xf_co_t                *co;             /*!< 下一级协程 */
    xf_co_state_t           state;          /*!< 状态 */
    uintptr_t               uip;            /*!< 无符号整型指针类型 (U Int Ptr) */
    void                   *vd;             /*!< 空指针类型数据 (Void Data) */
    xf_stimer_t            *stimer;         /*!< 定时器 */
} xf_co_temp_t;

/**
 * @brief 无栈协程上下文（系统上下文）基类。
 */
struct xf_co {
// public:
#if CONFIG_XF_CO_ENABLE_USER_DATA
    void                   *user_data;      /*!< 用户数据 */
#   define ud   user_data   /*!< 用户数据别名 */
#endif
// private:
    xf_co_func_t            func;           /*!< 协程执行函数 */
    xf_co_temp_t            temp;           /*!< 临时存储系统数据、转换目标等。 */
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
     *    类型见 @ref xf_co_state_t. 此处只可能是：
     *    - XF_CO_DEAD (默认)
     *    - XF_CO_NORMAL
     *    - XF_CO_RUNNING
     *    - XF_CO_SUSPENDED
     *
     * 2. B2: 等待位 (await).
     *
     *    正在等待子协程完成。为 1 时正在等待。
     *
     * 4. B3 ~ B5: 保留位 (reserved).
     *
     *    系统保留。
     *
     * 5. B6 ~ B12: 唯一 id.
     *
     *    范围 [0, 127]. id 同时标识优先级。
     *    数字越大，优先级越高。
     *
     * 2. B13 ~ B15: 协程类型 (type).
     *
     *    范围 [0, 7]. 用于标识用户上下文类型，见 @ref xf_co_type_t.
     *
     * @note 后缀：
     *      - S     起始位
     *      - W     宽度
     *      - B     位
     */
    xf_co_flags_t           flags;
#define XF_CO_FLAGS_STATE_S         (0)     /*!< 状态起始位 */
#define XF_CO_FLAGS_STATE_W         (2)     /*!< 宽度 */
#define XF_CO_FLAGS_AWAIT_B         (0x02)  /*!< 等待位 (B3) */
#define XF_CO_FLAGS_RESERVED_S      (3)     /*!< 保留位起始位 */
#define XF_CO_FLAGS_RESERVED_W      (3)     /*!< 宽度 */
#define XF_CO_FLAGS_ID_S            (6)     /*!< id 起始位 */
#define XF_CO_FLAGS_ID_W            (7)     /*!< 宽度 */
#define XF_CO_FLAGS_TYPE_S          (13)    /*!< 类型起始位 */
#define XF_CO_FLAGS_TYPE_W          (3)     /*!< 宽度 */
    /* TODO 对齐 */
};

/**
 * @brief 顶层协程类型。
 */
typedef struct xf_co_top {
    xf_co_t                 base;
    xf_co_t                *co_main;
    xf_co_state_t           co_main_state;
} xf_co_top_t;

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
    xf_list_node_t          node;           /*!< 平行协程节点 */
    uintptr_t               uip;            /*!< 无符号整型指针类型 (U Int Ptr)，
                                             *   可用作用户数据类型标识 */
    void                   *vd;             /*!< 空指针类型数据 (Void Data)，可用作用户数据 */
} xf_co_gen_t;

/* ==================== [Global Prototypes] ================================= */

xf_co_top_t *xf_co_get_top(void);
void xf_co_nest_depth_inc(void);
void xf_co_nest_depth_dec(void);
int8_t xf_co_get_nest_depth(void);

xf_err_t xf_co_ctor(
    xf_co_t *const co, xf_co_func_t func, void *user_data, xf_co_attr_t *p_attr);
xf_err_t xf_co_dtor(xf_co_t *const co);

void xf_stimer_call_co_cb(xf_stimer_t *const stimer);

/* ==================== [Macros] ============================================ */

#define xf_co_cast(_co)                 ((xf_co_t *)(_co))
#define xf_co_func_cast(_co_func)       ((xf_co_func_t)(_co_func))

#define xf_co_call(_co, _arg)           xf_co_cast(_co)->func(xf_co_cast(_co), (_arg))
#define xf_co_call_explicit(_co_func, _co, _arg) \
                                        xf_co_func_cast(_co_func)(xf_co_cast(_co), (_arg))

#define xf_co_get_flags_state(_co)      BITSn_GET_RSH(xf_co_cast(_co)->flags, \
                                            XF_CO_FLAGS_STATE_W, XF_CO_FLAGS_STATE_S)

#define xf_co_set_flags_state(_co, _value) \
                                        do { \
                                            BITSn_SET(xf_co_cast(_co)->flags, \
                                                      XF_CO_FLAGS_STATE_W, XF_CO_FLAGS_STATE_S, \
                                                      (_value)); \
                                        } while (0)

#define xf_co_get_flags_await_bit(_co)  BIT_GET(xf_co_cast(_co)->flags, \
                                                XF_CO_FLAGS_AWAIT_B)

#define xf_co_set_flags_await_bit(_co, _value) \
                                        do { \
                                            BIT_SET(xf_co_cast(_co)->flags, \
                                                    XF_CO_FLAGS_AWAIT_B, \
                                                    (_value)); \
                                        } while (0)

#define xf_co_get_flags_reserved(_co)   BITSn_GET_RSH(xf_co_cast(_co)->flags, \
                                            XF_CO_FLAGS_RESERVED_W, XF_CO_FLAGS_RESERVED_S)

#define xf_co_set_flags_reserved(_co, _value) \
                                        do { \
                                            BITSn_SET(xf_co_cast(_co)->flags, \
                                                      XF_CO_FLAGS_RESERVED_W, XF_CO_FLAGS_RESERVED_S, \
                                                      (_value)); \
                                        } while (0)

#define xf_co_get_flags_id(_co)         BITSn_GET_RSH(xf_co_cast(_co)->flags, \
                                            XF_CO_FLAGS_ID_W, XF_CO_FLAGS_ID_S)

#define xf_co_set_flags_id(_co, _value) \
                                        do { \
                                            BITSn_SET(xf_co_cast(_co)->flags, \
                                                      XF_CO_FLAGS_ID_W, XF_CO_FLAGS_ID_S, \
                                                      (_value)); \
                                        } while (0)

#define xf_co_get_flags_type(_co)       BITSn_GET_RSH(xf_co_cast(_co)->flags, \
                                            XF_CO_FLAGS_TYPE_W, XF_CO_FLAGS_TYPE_S)

#define xf_co_set_flags_type(_co, _value) \
                                        do { \
                                            BITSn_SET(xf_co_cast(_co)->flags, \
                                                      XF_CO_FLAGS_TYPE_W, XF_CO_FLAGS_TYPE_S, \
                                                      (_value)); \
                                        } while (0)

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __XF_CO_DEF_H__ */
