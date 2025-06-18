/**
 * @file xf_task_def.h
 * @author catcatBlue (catcatblue@qq.com)
 * @brief
 * @version 1.0
 * @date 2025-06-16
 *
 * SPDX-FileCopyrightText: 2025 CompanyNameMagicTag
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#ifndef __XF_TASK_DEF_H__
#define __XF_TASK_DEF_H__

/* ==================== [Includes] ========================================== */

#include "xf_common.h"

#include "xf_utils.h"

#include "xf_event.h"
#include "xf_ps.h"
#include "xf_stimer.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== [Defines] =========================================== */

/* ==================== [Typedefs] ========================================== */

/* 协程数量 */
/*
    ID 为 XF_TASK_NUM_MAX - 1 的协程默认被 xf_task_top 占用
 */
#define XF_TASK_NUM_MAX                   (16U)
/*
    maximum depth of state nesting in a coroutine (including the top level),
    must be >= 3
 */
#define XF_TASK_NEST_DEPTH_MAX            (6)

#if 1 /* def XF_TASK_LC_INCLUDE */
typedef void *xf_task_lc_t;
#define XF_TASK_LC_INIT_VALUE   NULL
#define xf_task_lc_init(s)      s = XF_TASK_LC_INIT_VALUE
#define xf_task_lc_resume(s)    do { if (s != XF_TASK_LC_INIT_VALUE) { goto *s; } } while (0)
#define xf_task_lc_set(s)       do { \
                                    XCAT2(XF_TASK_LC_LABEL, __LINE__): \
                                    (s) = &&XCAT2(XF_TASK_LC_LABEL, __LINE__); \
                                } while (0)
#define xf_task_lc_end(s)
#else
typedef uint16_t xf_task_lc_t;
#define XF_TASK_LC_INIT_VALUE   0
#define xf_task_lc_init(s)      s = XF_TASK_LC_INIT_VALUE;
#define xf_task_lc_resume(s)    switch(s) { /* fall-through */ /* FALLTHRU */ case XF_TASK_LC_INIT_VALUE:
#define xf_task_lc_set(s)       s = __LINE__; /* fall-through */ /* FALLTHRU */ case __LINE__:
#define xf_task_lc_end(s)       }
#endif

/**
 * @brief 任务唯一 ID 。
 */
typedef uint8_t xf_task_id_t;
#define XF_TASK_ID_INVALID              ((xf_task_id_t)~(xf_task_id_t)0) /*!< 无效任务 ID */

/**
 * @brief 无栈协程状态。
 *
 * 运行 xf_task_cb_t 后的返回值可能是：
 *      - XF_TASK_TERMINATED
 *      - XF_TASK_READY
 *      - XF_TASK_BLOCKED
 */
typedef uint8_t xf_task_state_t;
enum _xf_task_state_t {
    XF_TASK_TERMINATED       = 0x00U,       /*!< 已终止 */
    XF_TASK_READY,                          /*!< 就绪 */
    XF_TASK_BLOCKED,                        /*!< 阻塞 */
    XF_TASK_STATE_MAX,                      /*!< （不含）协程状态最大值 */
};

/**
 * @brief 无栈协程基类预声明。
 */
typedef struct xf_task xf_task_t;

/**
 * @brief 无栈协程回调函数。
 *
 * @param me    协程指针。实现函数时必须使用此名称。
 * @param arg   外部传入参数。实现函数时必须使用此名称。
 * @return xf_task_state_t 协程状态。
 *      - XF_TASK_TERMINATED    已终止
 *      - XF_TASK_READY         就绪
 *      - XF_TASK_BLOCKED       阻塞
 */
typedef xf_task_state_t (*xf_task_cb_t)(xf_task_t *me, void *arg);

/**
 * @brief 无栈协程属性。
 */
typedef volatile uint8_t xf_task_attr_t;

/**
 * @brief 无栈协程上下文（系统上下文）基类。
 */
struct xf_task {
// public:
    void                   *user_data;      /*!< 用户数据 */
// private:
    xf_task_cb_t            cb_func;        /*!< 协程执行函数 */
    /**
     * @brief Local Continuations（本地延续，当前代码的执行位置）。
     */
    volatile xf_task_lc_t   lc;
    xf_stimer_id_t          id_stimer;      /*!< 定时器 id */
    xf_ps_subscr_id_t       id_subscr;      /*!< 发布订阅 id */
    xf_task_id_t            id_parent;      /*!< 父任务 id */
    xf_task_id_t            id_child;       /*!< 子任务 id ，此处决定一个任务只能等一个子任务 */
    /**
     * @brief 协程属性 (attribute)。
     *
     * @details
     * 1. B0 ~ B1: 协程状态 (state).
     *
     *    类型见 @ref xf_task_state_t. 此处只可能是：
     *    - XF_TASK_TERMINATED (默认)
     *    - XF_TASK_READY
     *    - XF_TASK_RUNNING
     *    - XF_TASK_BLOCKED
     *
     * 2. B2 ~ B7: 保留位 (reserved).
     *
     *    系统保留。
     *
     * @note 后缀：
     *      - S     起始位
     *      - W     宽度
     *      - B     位
     */
    xf_task_attr_t            attr;
#define XF_TASK_ATTR_STATE_S          (0)     /*!< 状态起始位 */
#define XF_TASK_ATTR_STATE_W          (2)     /*!< 状态宽度 */
#define XF_TASK_ATTR_RESERVED_S       (2)     /*!< 保留位起始位 */
#define XF_TASK_ATTR_RESERVED_W       (6)     /*!< 保留位宽度 */
};

/* ==================== [Global Prototypes] ================================= */

/* ==================== [Macros] ============================================ */

#define xf_task_cast(_task)             ((xf_task_t *)(_task))
#define xf_task_cb_cast(_cb_func)       ((xf_task_cb_t)(_cb_func))

#define xf_task_run_direct(_task, _arg) xf_task_cast(_task)->cb_func(xf_task_cast(_task), (void *)(uintptr_t)(_arg))

/*
    NOTE 此处不使用 do {} while(0), 以便后续使用逗号表达式。
 */

#define xf_task_attr_get_state(_task)   BITSn_GET_RSH(xf_task_cast(_task)->attr, \
                                                      XF_TASK_ATTR_STATE_W, XF_TASK_ATTR_STATE_S)

#define xf_task_attr_set_state(_task, _value) \
                                        BITSn_SET(xf_task_cast(_task)->attr, \
                                                  XF_TASK_ATTR_STATE_W, XF_TASK_ATTR_STATE_S, \
                                                  (_value))

#define xf_task_attr_get_reserved(_task) \
                                        BITSn_GET_RSH(xf_task_cast(_task)->attr, \
                                                      XF_TASK_ATTR_RESERVED_W, XF_TASK_ATTR_RESERVED_S)

#define xf_task_attr_set_reserved(_task, _value) \
                                        BITSn_SET(xf_task_cast(_task)->attr, \
                                                  XF_TASK_ATTR_RESERVED_W, XF_TASK_ATTR_RESERVED_S, \
                                                  (_value))

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __XF_TASK_DEF_H__ */
