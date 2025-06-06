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

#define XF_PS_ELEM_SIZE                 (sizeof(xf_event_t *))

#define CONFIG_XF_PS_EVENT_NUM_MAX          (16U)   /*!< 默认通道中，最多存在的事件数量 */
#define CONFIG_XF_PS_SUBSCRIBER_NUM_MAX     (16U)   /*!< 默认池中，订阅者数量 */
#define CONFIG_XF_PS_SUBSCR_LIST_NUM_MAX    (2U)    /*!< 每个订阅者订阅事件 id 的最大数量 */

#define XF_PS_EVENT_NUM_MAX             CONFIG_XF_PS_EVENT_NUM_MAX      
#define XF_PS_SUBSCRIBER_NUM_MAX        CONFIG_XF_PS_SUBSCRIBER_NUM_MAX 
#define XF_PS_SUBSCR_LIST_NUM_MAX       CONFIG_XF_PS_SUBSCR_LIST_NUM_MAX

/* ==================== [Typedefs] ========================================== */

/* TODO 是否需要 id？用于插入通道链表时排序 */
typedef uint8_t xf_ps_id_t;

typedef struct xf_ps_channel {
    xf_list_head_t                      head;
    xf_dq_t                             event_queue;
} xf_ps_ch_t;

typedef struct xf_ps_subscriber xf_ps_subscr_t;

typedef void (*xf_ps_subscr_cb_t)(xf_ps_subscr_t *const me, xf_event_t *e);

struct xf_ps_subscriber {
    xf_list_node_t                      node;
    xf_ps_subscr_cb_t                   cb_func;
    void                               *user_data;
    xf_event_id_t                       subscr_list[XF_PS_SUBSCR_LIST_NUM_MAX];
};

/* ==================== [Global Prototypes] ================================= */

/* 通用发布订阅接口 */

/* 初始化一个发布订阅通道 */
xf_err_t xf_ps_init_ch(xf_ps_ch_t *const ch,
                       void *p_event_pool, xf_dq_size_t pool_size_bytes);

/* 获取一个未初始化的订阅者对象 */
xf_ps_subscr_t *xf_ps_acquire_subscriber(void);
/* 释放订阅者对象 */
xf_err_t xf_ps_release_subscriber(xf_ps_subscr_t *s);

/* 初始化订阅者对象 */
xf_err_t xf_ps_subscriber_init(
    xf_ps_subscr_t *const s, xf_ps_subscr_cb_t cb_func, void *user_data);
xf_err_t xf_ps_subscriber_deinit(xf_ps_subscr_t *const s);

/* 将订阅者对象绑定到指定发布订阅通道 */
xf_err_t xf_ps_subscriber_attach(
    xf_ps_ch_t *const ch, xf_ps_subscr_t *const s);
/* 从指定通道解绑订阅者 */
xf_err_t xf_ps_subscriber_detach(
    xf_ps_ch_t *const ch, xf_ps_subscr_t *const s);

/*
    订阅与取消订阅是通用的，直接使用后面声明的接口
    xf_ps_subscribe
    xf_ps_unsubscribe
 */

/* 将指定事件发布到指定通道 */
xf_err_t xf_ps_publish_to(xf_ps_ch_t *const ch, xf_event_t *const e);
/* 在指定通道立即发布事件给订阅者 */
xf_err_t xf_ps_publish_immediately_to(xf_ps_ch_t *const ch, xf_event_t *const e);

// /* 以后进先出的方式，将指定事件发布到指定通道 */
// xf_err_t xf_ps_publish_lifo_to(xf_ps_ch_t *const ch, xf_event_t *const e);
// /* 以后进先出的方式，在指定通道立即发布事件给订阅者 */
// xf_err_t xf_ps_publish_lifo_immediately_to(
//     xf_ps_ch_t *const ch, xf_event_t *const e);

/* 派发指定通道的消息 */
xf_err_t xf_ps_dispatch_ch(xf_ps_ch_t *const ch);

/* 获取默认发布订阅通道 */
xf_ps_ch_t *xf_ps_get_default_ch(void);

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
xf_err_t xf_ps_publish_immediately(xf_event_t *const e);

/* 在默认发布订阅通道中派发事件 */
xf_err_t xf_ps_dispatch(void);

/* ==================== [Macros] ============================================ */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __XF_PS_H__ */
