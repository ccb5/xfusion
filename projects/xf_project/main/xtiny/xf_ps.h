/**
 * @file xf_ps.h
 * @author catcatBlue (catcatblue@qq.com)
 * @brief
 * @version 1.0
 * @date 2025-06-02
 *
 * SPDX-FileCopyrightText: 2025 CompanyNameMagicTag
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#ifndef __XF_PS_H__
#define __XF_PS_H__

/* ==================== [Includes] ========================================== */

#include "xf_utils.h"
#include "xf_event.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== [Defines] =========================================== */

#define CONFIG_XF_PS_MSG_NUM_MAX                (16U)   /*!< 最大事件数量 */
#define CONFIG_XF_PS_SUBSCRIBER_NUM_MAX         (16U)   /*!< 订阅者数量 */

#define XF_PS_MSG_NUM_MAX               CONFIG_XF_PS_MSG_NUM_MAX
#define XF_PS_SUBSCRIBER_NUM_MAX        CONFIG_XF_PS_SUBSCRIBER_NUM_MAX

/* ==================== [Typedefs] ========================================== */

typedef uint8_t xf_ps_subscr_id_t;
#define XF_PS_ID_INVALID ((xf_ps_subscr_id_t)~(xf_ps_subscr_id_t)0) /*!< 无效订阅者 ID */

typedef struct xf_ps_subscriber xf_ps_subscr_t;

typedef struct xf_ps_info {
    xf_ps_subscr_t                     *s;          /*!< 订阅者 */
    uint8_t                             ref_cnt;    /*!< 剩余引用计数 */
} xf_ps_info_t;

/* 订阅者回调 */
typedef void (*xf_ps_subscr_cb_t)(xf_ps_info_t *info, void *arg);

struct xf_ps_subscriber {
    xf_event_id_t                       id;
    xf_ps_subscr_cb_t                   cb_func;
    void                               *user_data;
};

/* ==================== [Global Prototypes] ================================= */

xf_err_t xf_ps_init(void);

xf_ps_subscr_id_t xf_ps_subscribe(
    xf_event_id_t id, xf_ps_subscr_cb_t cb_func, void *user_data);
xf_err_t xf_ps_unsubscribe(xf_event_id_t id, xf_ps_subscr_cb_t cb_func);
xf_err_t xf_ps_unsubscribe_by_id(xf_ps_subscr_id_t subscr_id);

xf_err_t xf_ps_publish(xf_event_id_t id, void *arg);
xf_err_t xf_ps_publish_sync(xf_event_id_t id, void *arg);
/* TODO xf_ps_publish_safe */
// xf_err_t xf_ps_publish_safe(xf_event_id_t id, void *arg);

xf_err_t xf_ps_dispatch(void);

/* ==================== [Macros] ============================================ */

#define xf_subscribe(_id, _cb_func, _user_data) \
                                        xf_ps_subscribe((xf_event_id_t)(_id), (xf_ps_subscr_cb_t)(_cb_func), (void *)(uintptr_t)(_user_data))
#define xf_unsubscribe(_id, _cb_func)   xf_ps_unsubscribe((xf_event_id_t)(_id), (xf_ps_subscr_cb_t)(_cb_func))
#define xf_unsubscribe_by_id(_subscr_id) \
                                        xf_ps_unsubscribe_by_id((xf_ps_subscr_id_t)(_subscr_id))
#define xf_publish(_id, _arg)           xf_ps_publish((xf_event_id_t)(_id), (void *)(uintptr_t)(_arg))
#define xf_publish_sync(_id, _arg)      xf_ps_publish_sync((xf_event_id_t)(_id), (void *)(uintptr_t)(_arg))
#define xf_publish_safe(_id, _arg)      xf_ps_publish_safe((xf_event_id_t)(_id), (void *)(uintptr_t)(_arg))
#define xf_dispatch()                   xf_ps_dispatch()

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __XF_PS_H__ */
