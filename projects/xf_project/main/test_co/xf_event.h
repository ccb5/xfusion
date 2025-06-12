/**
 * @file xf_event.h
 * @author catcatBlue (catcatblue@qq.com)
 * @brief
 * @version 1.0
 * @date 2025-05-21
 *
 * SPDX-FileCopyrightText: 2025 CompanyNameMagicTag
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#ifndef __XF_EVENT_H__
#define __XF_EVENT_H__

/* ==================== [Includes] ========================================== */

#include "xf_common.h"
#include "xf_dstruct.h"
#include "xf_log.h"
#include "xf_check.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== [Defines] =========================================== */

#define CONFIG_XF_EVENT_GC_NUM_MAX      (8U)    /*!< 事件池/事件回收回调的最大数量，至少大于 2 */
#define XF_EVENT_GC_NUM_MAX             CONFIG_XF_EVENT_GC_NUM_MAX

#define XF_EVENT_ID_NUM_MAX             (128U)      /*!< xf_event 管理的事件 ID 的最大数量 */
#define XF_EVENT_ID_OFFSET              (32768U)    /*!< xf_event 管理的事件 ID 的默认偏移量 */

#if !defined(CONFIG_XF_EVENT_ID_SIZE)
#   define XF_EVENT_ID_SIZE             (2U)    /*!< 事件 ID 的默认大小，不建议修改 */
#else
#   define XF_EVENT_ID_SIZE             CONFIG_XF_EVENT_ID_SIZE
#endif

#define XF_EVENT_ID_INVALID             ((xf_event_id_t)~(xf_event_id_t)0) /*!< 无效事件 ID */

/* ==================== [Typedefs] ========================================== */

#if (XF_EVENT_ID_SIZE == 1U)
typedef uint8_t xf_event_id_t;
#elif (XF_EVENT_ID_SIZE == 2U)
typedef uint16_t xf_event_id_t;
#elif (XF_EVENT_ID_SIZE == 4U)
typedef uint32_t xf_event_id_t;
#elif (XF_EVENT_ID_SIZE == 8U)
typedef uint64_t xf_event_id_t;
#else
typedef uintptr_t xf_event_id_t;
#endif

enum _xf_event_id_t {
    XF_EVENT_ID_USER = 0x00,            /*!< 用户事件 ID 起始 */
    /* cppcheck-suppress misra-c2012-10.6 */
    XF_EVENT_ID_MAX = (xf_event_id_t)~((xf_event_id_t)0),  /*!< 最大事件 ID （含） */
};

typedef volatile uint8_t xf_event_ref_cnt_t;
typedef uint8_t xf_event_attr_t;

typedef uint8_t xf_event_pool_id_t;
enum _xf_event_pool_id_t {
    XF_EVENT_POOL_ID_BASE   = 0x00,     /*!< 事件池 ID 起始 */
    XF_EVENT_POOL_ID_STATIC = XF_EVENT_POOL_ID_BASE,  /*!< 全局事件，无需回收 */
    XF_EVENT_POOL_ID_DYN,               /*!< 动态事件，使用 xf_malloc 创建，默认使用 xf_free 回收 */
    /* TODO 添加内置池 ID */
    XF_EVENT_POOL_ID_USER,              /*!< 用户事件池 ID 起始 */
    XF_EVENT_POOL_ID_MAX    = 16,       /*!< 最大事件池 ID （不含） */
};

/* 事件基类预声明 */
typedef struct xf_event xf_event_t;

/* 事件 gc 回调 */
typedef xf_err_t (*xf_event_gc_cb_t)(xf_event_t *const e);

/**
 * @brief 事件基类。
 */
struct xf_event {
// public:
    xf_event_id_t           id;         /*!< 事件 ID */
// protected:
// private:
    xf_event_ref_cnt_t      ref_cnt;    /*!< 引用计数 */
    /**
     * @brief 事件属性 (attribute)。
     *
     * @details
     * 1. B0 ~ B3: 事件池 ID (pool_id).
     *
     *    当前事件所在的事件池的 ID 。
     *    在 xf_ps 中，当事件引用计数归零后，会调用相应事件池的 GC 函数回收事件。
     *
     * 2. B4 ~ B7: 保留位 (reserved).
     *
     *    系统保留。
     *
     * @note 后缀：
     *      - S     起始位
     *      - W     宽度
     *      - B     位
     */
    xf_event_attr_t         attr;
#define XF_EVENT_ATTR_POOL_ID_S            (0)     /*!< 事件池 ID 起始位 */
#define XF_EVENT_ATTR_POOL_ID_W            (4)     /*!< 事件池 ID 宽度 */
};
typedef xf_bitmap_t xf_event_bitmap_t;

/* ==================== [Global Prototypes] ================================= */

/* 获取唯一事件 id */
xf_event_id_t xf_event_acquire_id(void);
xf_err_t xf_event_release_id(xf_event_id_t id);

xf_err_t xf_event_gc_init(void);

xf_err_t xf_event_gc(xf_event_t *const e);
xf_err_t xf_event_gc_force(xf_event_t *const e);

xf_err_t xf_event_pool_set_gc_cb(xf_event_pool_id_t id,
                                 xf_event_gc_cb_t cb_func);

xf_event_gc_cb_t xf_event_pool_get_gc_cb(xf_event_pool_id_t id);

// /* TODO xf_event_malloc, xf_event_free */
// xf_event_t *xf_event_malloc(size_t size);
// void xf_event_free(void *e);

xf_err_t xf_event_init(xf_event_t *e, xf_event_id_t id, xf_event_pool_id_t id_pool);
xf_err_t xf_event_set_pool_id(xf_event_t *const e, xf_event_pool_id_t id);
xf_event_pool_id_t xf_event_get_pool_id(xf_event_t *const e);

/* ==================== [Macros] ============================================ */

#define xf_event_cast(_e)               ((xf_event_t *)(_e))

#define xf_event_attr_get_pool_id(_e)   BITSn_GET_RSH(xf_event_cast(_e)->attr, \
                                                      XF_EVENT_ATTR_POOL_ID_W, XF_EVENT_ATTR_POOL_ID_S)

#define xf_event_attr_set_pool_id(_e, _value) \
                                        do { \
                                            BITSn_SET(xf_event_cast(_e)->attr, \
                                                      XF_EVENT_ATTR_POOL_ID_W, XF_EVENT_ATTR_POOL_ID_S, \
                                                      (_value)); \
                                        } while (0)

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __XF_EVENT_H__ */
