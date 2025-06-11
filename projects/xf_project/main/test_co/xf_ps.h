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

#include "xf_ps_def.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== [Defines] =========================================== */

/* ==================== [Typedefs] ========================================== */

/* ==================== [Global Prototypes] ================================= */

/* 默认发布订阅通道 */

/* 初始化默认发布订阅通道 */
xf_err_t xf_ps_init(void);

/* 创造一个订阅者对象 = 获取订阅者对象 + 初始化 + attach 到默认发布订阅通道 */
xf_ps_subscr_t *xf_ps_create_subscriber(
    xf_ps_subscr_cb_t cb_func, void *user_data);
/* 销毁一个订阅者对象 = 在默认发布订阅通道中 detach + 反初始化 + 释放订阅者对象 */
xf_err_t xf_ps_destroy_subscriber(xf_ps_subscr_t *s);

/* 订阅一个事件 */
xf_err_t xf_ps_subscribe(xf_ps_subscr_t *const s, xf_event_id_t event_id);
/* 取消订阅一个事件 */
xf_err_t xf_ps_unsubscribe(xf_ps_subscr_t *const s, xf_event_id_t event_id);

/* 在默认发布订阅通道中发布事件 */
xf_err_t xf_ps_publish(xf_event_t *const e);
/* 在默认发布订阅通道中发布事件并立即派发 */
xf_err_t xf_ps_publish_immediately(xf_event_t *const e);

/* 在默认发布订阅通道中派发事件 */
xf_err_t xf_ps_dispatch(void);

/* ==================== [Macros] ============================================ */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __XF_PS_H__ */
