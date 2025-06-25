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

#include "../../common/xf_common.h"
#include "../../dstruct/xf_dstruct.h"
#include "../../log/xf_log.h"
#include "../safe/xf_safe.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== [Defines] =========================================== */

#define XF_EVENT_ID_NUM_MAX             (64U)       /*!< xf_event 管理的事件 ID 的最大数量 */
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
    XF_EVENT_ID_MAX = (xf_event_id_t)~((xf_event_id_t)0),  /*!< 最大事件 ID （不含） */
};

typedef struct xf_event_msg {
    xf_event_id_t           id;         /*!< 事件 ID */
    void                   *arg;        /*!< 事件参数 */
} xf_event_msg_t;

/* ==================== [Global Prototypes] ================================= */

/* 获取唯一事件 id */
xf_event_id_t xf_event_acquire_id(void);
xf_err_t xf_event_release_id(xf_event_id_t id);

/* ==================== [Macros] ============================================ */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __XF_EVENT_H__ */
