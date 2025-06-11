/**
 * @file xf_ps_def.h
 * @author catcatBlue (catcatblue@qq.com)
 * @brief
 * @version 1.0
 * @date 2025-06-11
 *
 * SPDX-FileCopyrightText: 2025 CompanyNameMagicTag
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#ifndef __XF_PS_DEF_H__
#define __XF_PS_DEF_H__

/* ==================== [Includes] ========================================== */

#include "xf_utils.h"
#include "xf_event.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== [Defines] =========================================== */

#define XF_PS_ELEM_SIZE                 (sizeof(xf_event_t *))

#define CONFIG_XF_PS_EVENT_NUM_MAX              (16U)   /*!< 默认通道中，最多存在的事件数量 */
#define CONFIG_XF_PS_SUBSCRIBER_NUM_MAX         (16U)   /*!< 默认池中，订阅者数量 */
#define CONFIG_XF_PS_SUBSCR_ID_LIST_NUM_MAX     (2U)    /*!< 每个订阅者订阅事件 id 的最大数量 */

#define XF_PS_EVENT_NUM_MAX             CONFIG_XF_PS_EVENT_NUM_MAX
#define XF_PS_SUBSCRIBER_NUM_MAX        CONFIG_XF_PS_SUBSCRIBER_NUM_MAX
#define XF_PS_SUBSCR_ID_LIST_NUM_MAX    CONFIG_XF_PS_SUBSCR_ID_LIST_NUM_MAX

/* ==================== [Typedefs] ========================================== */

/* TODO 是否需要 id？用于插入通道链表时排序 */
typedef uint8_t xf_ps_id_t;

/* 预声明 */
typedef struct xf_ps_channel xf_ps_ch_t;
typedef struct xf_ps_subscriber xf_ps_subscr_t;

/* 订阅者回调 */
typedef void (*xf_ps_subscr_cb_t)(xf_ps_subscr_t *const s, xf_event_t *e);

struct xf_ps_channel {
    xf_list_head_t                      head;
    xf_dq_t                             event_queue;
};

struct xf_ps_subscriber {
    xf_list_node_t                      node;
    xf_ps_subscr_cb_t                   cb_func;
    void                               *user_data;
    /* TODO 考虑移除订阅 ID 列表， xf_ps_subscr_t 作为基类尾随不同长度的列表 */
    xf_event_id_t                       subscr_id_list[XF_PS_SUBSCR_ID_LIST_NUM_MAX];
};

/* ==================== [Global Prototypes] ================================= */

/* 通用发布订阅接口 */

/* 获取事件引用计数 */
int32_t xf_ps_get_event_ref_cnt(xf_ps_ch_t *const ch, xf_event_id_t id);
xf_err_t xf_ps_update_event_ref_cnt(xf_ps_ch_t *const ch, xf_event_t *const e);

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

/* ==================== [Macros] ============================================ */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __XF_PS_DEF_H__ */
