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

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== [Defines] =========================================== */

#define XF_EVENT_ID_NUM_MAX             (128U)
#define XF_EVENT_ID_OFFSET              (32768U)

#if !defined(CONFIG_XF_EVENT_ID_SIZE)
#   define XF_EVENT_ID_SIZE             (2U)
#else
#   define XF_EVENT_ID_SIZE             CONFIG_XF_EVENT_ID_SIZE
#endif

#define XF_EVENT_ID_INVALID             ((xf_event_id_t)~(xf_event_id_t)0U)

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
    XF_EVENT_ID_USER = 0x00U,            /*!< 用户事件起始 */
    /* 最大事件 ID （含） */
    /* cppcheck-suppress misra-c2012-10.6 */
    XF_EVENT_ID_MAX = (xf_event_id_t)~((xf_event_id_t)0),
};

typedef volatile uint8_t xf_event_ref_cnt_t;
typedef volatile uint8_t xf_event_attr_t;

/**
 * @brief 事件基类。
 */
typedef struct xf_event {
// public:
    xf_event_id_t           id;         /*!< 事件 ID */
// protected:
// private:
    xf_event_ref_cnt_t      ref_cnt;    /*!< 引用计数 */
    xf_event_attr_t         attr;       /*!< 事件属性 */
} xf_event_t;

typedef xf_bitmap_t xf_event_bitmap_t;

/* ==================== [Global Prototypes] ================================= */

/* 获取唯一事件 id */
xf_event_id_t xf_event_acquire_id(void);
xf_err_t xf_event_release_id(xf_event_id_t eid);

/* ==================== [Macros] ============================================ */

#define xf_event_cast(_e)               ((xf_event_t *)(_e))

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __XF_EVENT_H__ */
