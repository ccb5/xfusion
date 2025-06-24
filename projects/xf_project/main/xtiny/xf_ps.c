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

#define XF_PS_ELEM_SIZE                 (sizeof(xf_event_msg_t))

/* ==================== [Static Prototypes] ================================= */

static xf_err_t xf_ps_notify(xf_event_msg_t *msg);

static xf_ps_subscr_t *xf_ps_acquire_subscriber(void);

#define xf_ps_release_subscriber(s) /*!< 什么也不做 */

static void xf_ps_subscriber_init(
    xf_ps_subscr_t *s,
    xf_event_id_t event_id, xf_ps_subscr_cb_t cb_func, void *user_data);

static void xf_ps_subscriber_deinit(xf_ps_subscr_t *s);

static uint8_t xf_ps_get_event_ref_cnt(xf_event_id_t event_id);

/* ==================== [Static Variables] ================================== */

static const char *const TAG = "xf_ps";

/* 订阅者池 */
static xf_ps_subscr_t s_subscr_pool[XF_PS_SUBSCRIBER_NUM_MAX] = {0};

/* 默认通道及其事件池 */
static xf_ps_ch_t s_default_ch = {0};
static xf_ps_ch_t *const sp_ch = &s_default_ch;

/* 消息池 */
static xf_event_msg_t s_msg_pool[XF_PS_MSG_NUM_MAX] = {0};

/* ==================== [Macros] ============================================ */

/* ==================== [Global Functions] ================================== */

xf_err_t xf_ps_init(void)
{
    if (sp_ch->event_queue.buf_size == 0) {
        xf_deque_init(&sp_ch->event_queue, s_msg_pool, sizeof(s_msg_pool));
    }
    return XF_OK;
}

xf_ps_subscr_t *xf_ps_subscribe(
    xf_event_id_t event_id, xf_ps_subscr_cb_t cb_func, void *user_data)
{
    xf_ps_subscr_t *s;
    /*
        应当允许同一个回调订阅两次相同事件 id.
        例如：
            xf_task 中的 xf_resume_task_subscr_cb, 不同的 task 可能会订阅同一个事件 id.
            但是 user_data 对应不同的 task.
     */
#if 0
    uint8_t i;
    /* 查找 cb_func 是否已经订阅过当前事件 id, 同一个回调不允许订阅两次相同事件 id */
    for (i = 0; i < XF_PS_SUBSCRIBER_NUM_MAX; i++) {
        if ((s_subscr_pool[i].event_id == event_id)
                && (s_subscr_pool[i].cb_func == cb_func)) {
            return &s_subscr_pool[i];
        }
    }
#endif
    s = xf_ps_acquire_subscriber();
    if (s == NULL) {
        XF_ERROR_LINE(); XF_LOGD(TAG, "no subscriber");
        return NULL;
    }
    xf_ps_subscriber_init(s, event_id, cb_func, user_data);
    return s;
}

xf_err_t xf_ps_unsubscribe(
    xf_event_id_t event_id, xf_ps_subscr_cb_t cb_func, void *user_data)
{
    uint8_t i;
    bool_t match_event = (event_id != XF_EVENT_ID_INVALID) ? TRUE : FALSE;
    bool_t match_cb = (cb_func != NULL) ? TRUE : FALSE;
    bool_t match_user = (user_data != (void *)XF_PS_USER_DATA_INVALID) ? TRUE : FALSE;
    bool_t found = FALSE;
    /* 无效参数检查 */
    if (!match_cb && !match_event) {
        return XF_ERR_INVALID_ARG;
    }
    /* 统一处理所有订阅者 */
    for (i = 0; i < XF_PS_SUBSCRIBER_NUM_MAX; i++) {
        bool_t match = TRUE;
        /* 事件匹配检查 */
        if (match_event
                && (s_subscr_pool[i].event_id != event_id)) {
            match = FALSE;
        }
        /* 回调函数匹配检查 */
        if (match && match_cb
                && (s_subscr_pool[i].cb_func != cb_func)) {
            match = FALSE;
        }
        /* 用户数据匹配检查 */
        if (match && match_cb && match_user
                && (s_subscr_pool[i].user_data != user_data)) {
            match = FALSE;
        }
        if (match) {
            xf_ps_subscriber_deinit(&s_subscr_pool[i]);
            xf_ps_release_subscriber(&s_subscr_pool[i]);
            found = TRUE;
            if (match_event && match_cb && match_user) {
                return XF_OK;
            }
        }
    }
    return found ? XF_OK : XF_ERR_NOT_FOUND;
}

xf_err_t xf_ps_unsubscribe_by_subscr(xf_ps_subscr_t *s)
{
    if ((s == NULL)
            || (s < &s_subscr_pool[0])
            || (s > &s_subscr_pool[XF_PS_SUBSCRIBER_NUM_MAX - 1])) {
        return XF_ERR_INVALID_ARG;
    }
    xf_ps_subscriber_deinit(s);
    xf_ps_release_subscriber(s);
    return XF_OK;
}

xf_err_t xf_ps_publish(xf_event_id_t event_id, void *arg)
{
    xf_event_msg_t msg = {0};
    uint8_t ref_cnt;
    xf_dq_size_t pushed_size;
    XF_CRIT_STAT();
    if (event_id == XF_EVENT_ID_INVALID) {
        return XF_ERR_INVALID_ARG;
    }
    ref_cnt = xf_ps_get_event_ref_cnt(event_id);
    if (ref_cnt == 0) {
        XF_ERROR_LINE(); XF_LOGD(TAG, "no subscriber");
        return XF_FAIL;
    }
    msg.id = event_id;
    msg.arg = arg;
    XF_CRIT_ENTRY();
    /* 加入事件队列 */
    pushed_size = xf_deque_back_push(&sp_ch->event_queue, (void *)&msg, XF_PS_ELEM_SIZE);
    XF_CRIT_EXIT();
    if (pushed_size != XF_PS_ELEM_SIZE) {
        XF_ERROR_LINE(); XF_LOGD(TAG, "push failed");
        return XF_ERR_NO_MEM;
    }
    return XF_OK;
}

xf_err_t xf_ps_publish_sync(xf_event_id_t event_id, void *arg)
{
    xf_event_msg_t msg = {0};
    if (event_id == XF_EVENT_ID_INVALID) {
        return XF_ERR_INVALID_ARG;
    }
    msg.id = event_id;
    msg.arg = arg;
    return xf_ps_notify(&msg);
}

xf_err_t xf_ps_dispatch(void)
{
    xf_err_t xf_ret = XF_FAIL;
    xf_dq_size_t filled_size;
    xf_dq_size_t popped_size;
    xf_event_msg_t msg = {0};
    XF_CRIT_STAT();
    XF_CRIT_ENTRY();
    filled_size = xf_deque_get_filled(&sp_ch->event_queue);
    XF_CRIT_EXIT();
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
        XF_CRIT_ENTRY();
        popped_size = xf_deque_front_pop(&sp_ch->event_queue,
                                         (void *)&msg, XF_PS_ELEM_SIZE);
        XF_CRIT_EXIT();
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

xf_ps_subscr_t *xf_ps_id_to_subscr(xf_ps_subscr_id_t subscr_id)
{
    if (subscr_id >= XF_PS_SUBSCRIBER_NUM_MAX) {
        return NULL;
    }
    return &s_subscr_pool[subscr_id];
}

/* ==================== [Static Functions] ================================== */

static xf_ps_subscr_t *xf_ps_acquire_subscriber(void)
{
    uint8_t i;
    XF_CRIT_STAT();
    XF_CRIT_ENTRY();
    for (i = 0; i < XF_PS_SUBSCRIBER_NUM_MAX; i++) {
        if (s_subscr_pool[i].cb_func == NULL) {
            s_subscr_pool[i].cb_func = XF_CRIT_PTR_UNINIT;
            XF_CRIT_EXIT();
            return &s_subscr_pool[i];
        }
    }
    XF_CRIT_EXIT();
    return NULL;
}

static void xf_ps_subscriber_init(
    xf_ps_subscr_t *s,
    xf_event_id_t event_id, xf_ps_subscr_cb_t cb_func, void *user_data)
{
    s->event_id = event_id;
    s->cb_func = cb_func;
    s->user_data = user_data;
}

static void xf_ps_subscriber_deinit(xf_ps_subscr_t *s)
{
    xf_memset(s, 0, sizeof(xf_ps_subscr_t));
}

static uint8_t xf_ps_get_event_ref_cnt(xf_event_id_t event_id)
{
    uint8_t ref_cnt = 0;
    uint8_t i;
    XF_CRIT_STAT();
    if (event_id == XF_EVENT_ID_INVALID) {
        return 0;
    }
    XF_CRIT_ENTRY();
    for (i = 0; i < XF_PS_SUBSCRIBER_NUM_MAX; i++) {
        if ((s_subscr_pool[i].cb_func)
                && (s_subscr_pool[i].cb_func != XF_CRIT_PTR_UNINIT)
                && (s_subscr_pool[i].event_id == event_id)) {
            ref_cnt++;
        }
    }
    XF_CRIT_EXIT();
    return ref_cnt;
}

static xf_err_t xf_ps_notify(xf_event_msg_t *msg)
{
    uint8_t i;
    uint8_t ref_cnt;
    XF_CRIT_STAT();
    ref_cnt = xf_ps_get_event_ref_cnt(msg->id);
    if (ref_cnt == 0) {
        return XF_FAIL;
    }
    for (i = 0; i < XF_PS_SUBSCRIBER_NUM_MAX; i++) {
        XF_CRIT_ENTRY();
        if (!(s_subscr_pool[i].cb_func)
                || (s_subscr_pool[i].cb_func == XF_CRIT_PTR_UNINIT)) {
            XF_CRIT_EXIT();
            continue;
        }
        XF_CRIT_EXIT();
        if (s_subscr_pool[i].event_id == msg->id) {
            --ref_cnt;
            s_subscr_pool[i].cb_func(&s_subscr_pool[i], ref_cnt, msg->arg);
            /* 
                TODO 如果有在回调中订阅或取消订阅，需要重新计算 ref_cnt
             */
        }
    }
    return XF_OK;
}
