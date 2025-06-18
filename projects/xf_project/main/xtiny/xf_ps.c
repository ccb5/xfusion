/**
 * @file xf_ps.c
 * @author catcatBlue (catcatblue@qq.com)
 * @brief
 * @version 1.0
 * @date 2025-06-02
 *
 * SPDX-FileCopyrightText: 2025 CompanyNameMagicTag
 * SPDX-License-Identifier: Apache-2.0
 *
 */

/* ==================== [Includes] ========================================== */

#include "xf_ps.h"

/* ==================== [Defines] =========================================== */

/* 内部循环变量均使用 uint8_t */
STATIC_ASSERT(XF_PS_SUBSCRIBER_NUM_MAX < ((uint8_t)~(uint8_t)0));

/* ==================== [Typedefs] ========================================== */

typedef struct xf_ps_channel {
    xf_dq_t                             event_queue;
} xf_ps_ch_t;

typedef uint16_t xf_event_id_t;
typedef struct xf_ps_msg {
    xf_event_id_t                       id;
    void                               *arg;
} xf_ps_msg_t;

#define XF_PS_ELEM_SIZE                 (sizeof(xf_ps_msg_t))

/* ==================== [Static Prototypes] ================================= */

static xf_err_t xf_ps_notify(xf_ps_msg_t *msg);

static xf_ps_subscr_t *xf_ps_acquire_subscriber(void);

#define xf_ps_release_subscriber(s) /*!< 什么也不做 */

static void xf_ps_subscriber_init(
    xf_ps_subscr_t *s,
    xf_event_id_t id, xf_ps_subscr_cb_t cb_func, void *user_data);

static void xf_ps_subscriber_deinit(xf_ps_subscr_t *s);

static uint8_t xf_ps_get_event_ref_cnt(xf_event_id_t id);

/* ==================== [Static Variables] ================================== */

static const char *const TAG = "xf_ps";

/* 订阅者池 */
static xf_ps_subscr_t s_subscr_pool[XF_PS_SUBSCRIBER_NUM_MAX] = {0};

/* 默认通道及其事件池 */
static xf_ps_ch_t s_default_ch = {0};
static xf_ps_ch_t *const sp_ch = &s_default_ch;

/* 消息池 */
static xf_ps_msg_t s_msg_pool[XF_PS_MSG_NUM_MAX] = {0};

/* ==================== [Macros] ============================================ */

/* ==================== [Global Functions] ================================== */

xf_err_t xf_ps_init(void)
{
    if (sp_ch->event_queue.buf_size == 0) {
        xf_deque_init(&sp_ch->event_queue, s_msg_pool, sizeof(s_msg_pool));
    }
    return XF_OK;
}

xf_ps_subscr_id_t xf_ps_subscribe(
    xf_event_id_t id, xf_ps_subscr_cb_t cb_func, void *user_data)
{
    uint8_t i;
    xf_ps_subscr_t *s;
    /* 查找 cb_func 是否已经订阅过当前事件 id, 同一个回调不允许订阅两次相同事件 id */
    for (i = 0; i < XF_PS_SUBSCRIBER_NUM_MAX; i++) {
        if ((s_subscr_pool[i].id == id)
                && (s_subscr_pool[i].cb_func == cb_func)) {
            return i;
        }
    }
    s = xf_ps_acquire_subscriber();
    if (s == NULL) {
        XF_ERROR_LINE(); XF_LOGD(TAG, "no subscriber");
        return XF_PS_ID_INVALID;
    }
    xf_ps_subscriber_init(s, id, cb_func, user_data);
    return xf_ps_subscr_to_id(s);
}

xf_err_t xf_ps_unsubscribe(xf_event_id_t id, xf_ps_subscr_cb_t cb_func)
{
    uint8_t i;
    if ((id == XF_PS_SUBSCRIBER_NUM_MAX) && (cb_func == NULL)) {
        return XF_ERR_INVALID_ARG;
    }
    /* 当前回调取消订阅所有事件 */
    if ((id == XF_PS_SUBSCRIBER_NUM_MAX) && (cb_func != NULL)) {
        for (i = 0; i < XF_PS_SUBSCRIBER_NUM_MAX; i++) {
            if (s_subscr_pool[i].cb_func == cb_func) {
                xf_ps_subscriber_deinit(&s_subscr_pool[i]);
                xf_ps_release_subscriber(&s_subscr_pool[i]);
            }
        }
        return XF_OK;
    }
    /* 取消订阅指定事件所有回调 */
    if ((id != XF_PS_SUBSCRIBER_NUM_MAX) && (cb_func == NULL)) {
        for (i = 0; i < XF_PS_SUBSCRIBER_NUM_MAX; i++) {
            if (s_subscr_pool[i].id == id) {
                xf_ps_subscriber_deinit(&s_subscr_pool[i]);
                xf_ps_release_subscriber(&s_subscr_pool[i]);
            }
        }
        return XF_OK;
    }
    /* 当前回调取消订阅指定事件 */
    for (i = 0; i < XF_PS_SUBSCRIBER_NUM_MAX; i++) {
        if ((s_subscr_pool[i].id == id)
                && (s_subscr_pool[i].cb_func == cb_func)) {
            xf_ps_subscriber_deinit(&s_subscr_pool[i]);
            xf_ps_release_subscriber(&s_subscr_pool[i]);
            return XF_OK;
        }
    }
    return XF_ERR_NOT_FOUND;
}

xf_err_t xf_ps_unsubscribe_by_id(xf_ps_subscr_id_t subscr_id)
{
    xf_ps_subscr_t *s;
    if (subscr_id >= XF_PS_SUBSCRIBER_NUM_MAX) {
        return XF_ERR_INVALID_ARG;
    }
    s = &s_subscr_pool[subscr_id];
    xf_ps_subscriber_deinit(s);
    xf_ps_release_subscriber(s);
    return XF_OK;
}

xf_err_t xf_ps_publish(xf_event_id_t id, void *arg)
{
    xf_ps_msg_t msg = {0};
    xf_dq_size_t pushed_size;
    if (id == XF_EVENT_ID_INVALID) {
        return XF_ERR_INVALID_ARG;
    }
    msg.id = id;
    msg.arg = arg;
    /* 加入事件队列 */
    pushed_size = xf_deque_back_push(&sp_ch->event_queue, (void *)&msg, XF_PS_ELEM_SIZE);
    if (pushed_size != XF_PS_ELEM_SIZE) {
        XF_ERROR_LINE(); XF_LOGD(TAG, "push failed");
        return XF_FAIL;
    }
    return XF_OK;
}

xf_err_t xf_ps_publish_sync(xf_event_id_t id, void *arg)
{
    xf_ps_msg_t msg = {0};
    if (id == XF_EVENT_ID_INVALID) {
        return XF_ERR_INVALID_ARG;
    }
    msg.id = id;
    msg.arg = arg;
    return xf_ps_notify(&msg);
}

xf_err_t xf_ps_dispatch(void)
{
    xf_err_t xf_ret = XF_FAIL;
    xf_dq_size_t filled_size;
    xf_dq_size_t popped_size;
    xf_ps_msg_t msg = {0};
    filled_size = xf_deque_get_filled(&sp_ch->event_queue);
    if (filled_size == 0) {
        return XF_OK;
    }
#if !defined(NDEBUG)
    if (filled_size % XF_PS_ELEM_SIZE) {
        /*
            正常情况下都是 XF_PS_ELEM_SIZE 的整数倍。
            XF_PS_ELEM_SIZE不一定是 2 的幂，不使用 IS_ALIGNED；
         */
        XF_FATAL_ERROR();
    }
#endif
    while (filled_size >= XF_PS_ELEM_SIZE) {
        popped_size = xf_deque_front_pop(&sp_ch->event_queue,
                                         (void *)&msg, XF_PS_ELEM_SIZE);
        if (unlikely(popped_size == 0)) {
            /* 说明有别处取走了，不算致命错误 */
            break;
        }
        if (unlikely((popped_size != XF_PS_ELEM_SIZE))) {
            XF_FATAL_ERROR();
        }
        xf_ret = xf_ps_notify(&msg);
        filled_size -= XF_PS_ELEM_SIZE;
    }
    return xf_ret;
}

xf_ps_subscr_id_t xf_ps_subscr_to_id(const xf_ps_subscr_t *s)
{
    if ((s == NULL)
            || (s < &s_subscr_pool[0])
            || (s > &s_subscr_pool[XF_PS_SUBSCRIBER_NUM_MAX - 1])) {
        return XF_PS_ID_INVALID;
    }
    return (xf_ps_subscr_id_t)(s - &s_subscr_pool[0]);
}

xf_ps_subscr_t *xf_ps_id_to_subscr(xf_ps_subscr_id_t id)
{
    if (id >= XF_PS_SUBSCRIBER_NUM_MAX) {
        return NULL;
    }
    return &s_subscr_pool[id];
}

/* ==================== [Static Functions] ================================== */

static xf_ps_subscr_t *xf_ps_acquire_subscriber(void)
{
    uint8_t i;
    for (i = 0; i < XF_PS_SUBSCRIBER_NUM_MAX; i++) {
        if (s_subscr_pool[i].cb_func == NULL) {
            return &s_subscr_pool[i];
        }
    }
    return NULL;
}

static void xf_ps_subscriber_init(
    xf_ps_subscr_t *s,
    xf_event_id_t id, xf_ps_subscr_cb_t cb_func, void *user_data)
{
    s->id = id;
    s->cb_func = cb_func;
    s->user_data = user_data;
}

static void xf_ps_subscriber_deinit(xf_ps_subscr_t *s)
{
    xf_memset(s, 0, sizeof(xf_ps_subscr_t));
}

static uint8_t xf_ps_get_event_ref_cnt(xf_event_id_t id)
{
    uint8_t ref_cnt = 0;
    uint8_t i;
    if (id == XF_EVENT_ID_INVALID) {
        return 0;
    }
    for (i = 0; i < XF_PS_SUBSCRIBER_NUM_MAX; i++) {
        if ((s_subscr_pool[i].cb_func) && (s_subscr_pool[i].id == id)) {
            ref_cnt++;
        }
    }
    return ref_cnt;
}

static xf_err_t xf_ps_notify(xf_ps_msg_t *msg)
{
    uint8_t i;
    xf_ps_info_t info = {0};
    info.ref_cnt = xf_ps_get_event_ref_cnt(msg->id);
    if (info.ref_cnt == 0) {
        return XF_FAIL;
    }
    for (i = 0; i < XF_PS_SUBSCRIBER_NUM_MAX; i++) {
        if (s_subscr_pool[i].cb_func == NULL) {
            continue;
        }
        if (s_subscr_pool[i].id == msg->id) {
            info.s = &s_subscr_pool[i];
            --info.ref_cnt;
            s_subscr_pool[i].cb_func(&info, msg->arg);
        }
    }
    return XF_OK;
}
