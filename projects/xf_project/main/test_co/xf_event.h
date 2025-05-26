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

/* ==================== [Typedefs] ========================================== */

#if !defined(CONFIG_XF_EVENT_ID_SIZE)
#   define XF_EVENT_ID_SIZE    2
#else
#   define XF_EVENT_ID_SIZE    CONFIG_XF_EVENT_ID_SIZE
#endif

#if (XF_EVENT_ID_SIZE == 1)
typedef uint8_t xf_event_id_t;
#elif (XF_EVENT_ID_SIZE == 2)
typedef uint16_t xf_event_id_t;
#elif (XF_EVENT_ID_SIZE == 4)
typedef uint32_t xf_event_id_t;
#elif (XF_EVENT_ID_SIZE == 8)
typedef uint64_t xf_event_id_t;
#else
typedef uintptr_t xf_event_id_t;
#endif

enum _xf_event_id_t {
    XF_EID_EMPTY = 0x00,
    XF_EID_USER,
    XF_EID_MAX,
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

/* ==================== [Macros] ============================================ */

#define xf_event_cast(_e)               ((xf_event_t *)(_e))

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __XF_EVENT_H__ */
