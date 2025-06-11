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

static const char *const TAG = "xf_ps";

/* ==================== [Typedefs] ========================================== */

/* ==================== [Static Prototypes] ================================= */

static xf_err_t xf_ps_notify(xf_ps_ch_t *const ch, xf_event_t *const e);

/* ==================== [Static Variables] ================================== */

/* 订阅者池及其位图管理 */
static xf_ps_subscr_t s_subscr_pool[XF_PS_SUBSCRIBER_NUM_MAX] = {0};
static xf_bitmap32_t s_subscr_pool_bm[XF_BITMAP32_GET_BLK_SIZE(XF_PS_SUBSCRIBER_NUM_MAX)] = {0};

/* 默认通道及其事件池 */
static xf_ps_ch_t s_default_ch = {0};
static xf_ps_ch_t *const sp_ch = &s_default_ch;
static xf_event_t *s_event_pool[XF_PS_EVENT_NUM_MAX] = {0};

static bool_t sb_subscriber_created = FALSE;
static bool_t sb_subscriber_deleted = FALSE;

/* ==================== [Macros] ============================================ */

/* ==================== [Global Functions] ================================== */

xf_err_t xf_ps_init_ch(xf_ps_ch_t *const ch,
                       void *p_event_pool, xf_dq_size_t pool_size_bytes)
{
    xf_err_t xf_ret;
    if ((ch == NULL)
            || (p_event_pool == NULL)
            || (pool_size_bytes == 0)
            || (!IS_ALIGNED(pool_size_bytes, XF_PS_ELEM_SIZE))) {
        return XF_ERR_INVALID_ARG;
    }
    xf_ret = xf_deque_init(&ch->event_queue,
                           p_event_pool, pool_size_bytes);
    xf_list_init(&ch->head);
    return xf_ret;
}

xf_ps_subscr_t *xf_ps_acquire_subscriber(void)
{
    int32_t idx;
    idx = xf_bitmap32_ffz(s_subscr_pool_bm, XF_PS_SUBSCRIBER_NUM_MAX);
    if (idx < 0) {
        return NULL;
    }
    XF_BITMAP32_SET1(s_subscr_pool_bm, idx);
    sb_subscriber_created = TRUE;
    return &s_subscr_pool[idx];
}

xf_err_t xf_ps_release_subscriber(xf_ps_subscr_t *s)
{
    intptr_t idx;
    if (s == NULL) {
        return XF_ERR_INVALID_ARG;
    }
    /* 检查是否是池内的定时器 */
    if ((s < &s_subscr_pool[0])
            || (s > &s_subscr_pool[XF_PS_SUBSCRIBER_NUM_MAX - 1])) {
        return XF_ERR_INVALID_ARG;
    }
    idx = ((intptr_t)s - (intptr_t)&s_subscr_pool[0]) / (sizeof(s_subscr_pool[0]));
    if (XF_BITMAP32_GET(s_subscr_pool_bm, idx) == 0) {
        return XF_ERR_INVALID_ARG;
    }
    XF_BITMAP32_SET0(s_subscr_pool_bm, idx);
    sb_subscriber_deleted = TRUE;
    return XF_OK;
}

xf_err_t xf_ps_subscriber_init(
    xf_ps_subscr_t *const s, xf_ps_subscr_cb_t cb_func, void *user_data)
{
    uint32_t i;
    if ((s == NULL) || (cb_func == NULL)) {
        return XF_ERR_INVALID_ARG;
    }
    s->cb_func = cb_func;
    s->user_data = user_data;
    for (i = 0; i < XF_PS_SUBSCR_ID_LIST_NUM_MAX; i++) {
        s->subscr_id_list[i] = XF_EVENT_ID_INVALID;
    }
    xf_list_init(&s->node);
    return XF_OK;
}

xf_err_t xf_ps_subscriber_deinit(xf_ps_subscr_t *const s)
{
    const xf_ps_subscr_t subscr_empty = {0};
    if (s == NULL) {
        return XF_ERR_INVALID_ARG;
    }
    /* 此实现只适用于双链，单链表时必须知道表头 */
    xf_list_del(&s->node);
    // xf_list_init(&s->node); ///!< 不需要再次初始化
    *s = subscr_empty;
    return XF_OK;
}

xf_err_t xf_ps_subscriber_attach(
    xf_ps_ch_t *const ch, xf_ps_subscr_t *const s)
{
    bool_t b_found = FALSE;
    if ((ch == NULL) || (s == NULL)) {
        return XF_ERR_INVALID_ARG;
    }
    /* 如果 s 未挂链表 */
    if ((xf_list_empty_careful(&s->node))
            || ((s->node.next == (xf_list_t *)XF_LIST_POISON1)
                && (s->node.prev == (xf_list_t *)XF_LIST_POISON2))) {
        xf_list_add_tail(&s->node, &ch->head);
    } else {
        /* s 已挂链表，检查是否在当前表上，在则跳过，不在则挂上 */
        xf_list_node_t *pos;
        xf_list_node_t *n;
        xf_list_for_each_safe(pos, n, &ch->head) {
            if (pos == &s->node) {
                b_found = TRUE;
                break;
            }
        }
        if (!b_found) {
            /* 节点已挂在其他链表 */
            xf_list_del(&s->node);
            xf_list_add_tail(&s->node, &ch->head);
        }
    }
    return XF_OK;
}

xf_err_t xf_ps_subscriber_detach(
    xf_ps_ch_t *const ch, xf_ps_subscr_t *const s)
{
    xf_list_node_t *pos;
    xf_list_node_t *n;
    if ((ch == NULL) || (s == NULL)) {
        return XF_ERR_INVALID_ARG;
    }
    /* 如果 s 未挂链表 */
    if ((xf_list_empty_careful(&s->node))
            || ((s->node.next == (xf_list_t *)XF_LIST_POISON1)
                && (s->node.prev == (xf_list_t *)XF_LIST_POISON2))) {
        return XF_OK;
    }
    /* s 已挂链表，检查是否在当前表上，在则删除，不在则返回未找到 */
    xf_list_for_each_safe(pos, n, &ch->head) {
        if (pos == &s->node) {
            xf_list_del(&s->node);
            xf_list_init(&s->node);
            return XF_OK;
        }
    }
    return XF_ERR_NOT_FOUND;
}

xf_err_t xf_ps_subscribe(xf_ps_subscr_t *const s, xf_event_id_t event_id)
{
    uint32_t i;
    uint32_t free_idx = XF_PS_SUBSCR_ID_LIST_NUM_MAX;
    if ((s == NULL) || (event_id == XF_EVENT_ID_INVALID)) {
        return XF_ERR_INVALID_ARG;
    }
    /* 检查该订阅者是否已经订阅过此 event_id，已订阅则返回，未订阅则寻找空位 */
    for (i = 0; i < XF_PS_SUBSCR_ID_LIST_NUM_MAX; i++) {
        if (s->subscr_id_list[i] == event_id) {
            /* NOTE 此处不允许订阅两次相同事件 ID */
            return XF_OK;
        }
        if ((free_idx == XF_PS_SUBSCR_ID_LIST_NUM_MAX)
                && (s->subscr_id_list[i] == XF_EVENT_ID_INVALID)) {
            free_idx = i;
        }
    }
    if (free_idx == XF_PS_SUBSCR_ID_LIST_NUM_MAX) {
        return XF_ERR_RESOURCE;
    }
    s->subscr_id_list[free_idx] = event_id;
    return XF_OK;
}

xf_err_t xf_ps_unsubscribe(xf_ps_subscr_t *const s, xf_event_id_t event_id)
{
    uint32_t i;
    if ((s == NULL) || (event_id == XF_EVENT_ID_INVALID)) {
        return XF_ERR_INVALID_ARG;
    }
    /* 遍历该订阅者的订阅列表，寻找对应的 event_id */
    for (i = 0; i < XF_PS_SUBSCR_ID_LIST_NUM_MAX; i++) {
        if (s->subscr_id_list[i] == event_id) {
            s->subscr_id_list[i] = XF_EVENT_ID_INVALID;
            return XF_OK;
        }
    }
    return XF_ERR_NOT_FOUND;
}

xf_err_t xf_ps_publish_to(xf_ps_ch_t *const ch, xf_event_t *const e)
{
    xf_err_t xf_ret;
    xf_dq_size_t pushed_size;
    if ((ch == NULL) || (e == NULL)) {
        return XF_ERR_INVALID_ARG;
    }
    xf_ret = xf_ps_update_event_ref_cnt(ch, e);
    if (xf_ret != XF_OK) {
        XF_LOGD(TAG, "xf_ps_update_event_ref_cnt failed");
        return xf_ret;
    }
    /* 加入事件队列， WARNING 此处仅拷贝指针 */
    pushed_size = xf_deque_back_push(&ch->event_queue, (void *)&e, XF_PS_ELEM_SIZE);
    if (pushed_size != XF_PS_ELEM_SIZE) {
        XF_ERROR_LINE();
        return XF_FAIL;
    }
    return XF_OK;
}

xf_err_t xf_ps_publish_immediately_to(
    xf_ps_ch_t *const ch, xf_event_t *const e)
{
    xf_err_t xf_ret;
    if ((ch == NULL) || (e == NULL)) {
        return XF_ERR_INVALID_ARG;
    }
    xf_ret = xf_ps_update_event_ref_cnt(ch, e);
    if (xf_ret != XF_OK) {
        XF_LOGD(TAG, "xf_ps_update_event_ref_cnt failed");
        return xf_ret;
    }
    xf_ret = xf_ps_notify(ch, e);
    if (xf_ret != XF_OK) {
        XF_ERROR_LINE();
    }
    return xf_ret;
}

int32_t xf_ps_get_event_ref_cnt(xf_ps_ch_t *const ch, xf_event_id_t id)
{
    xf_ps_subscr_t *pos;
    int32_t ref_cnt = 0;
    uint32_t i;
    if ((ch == NULL) || (id == XF_EVENT_ID_INVALID)) {
        return -1;
    }
    /* 统计当前通道中订阅该事件的订阅者数量 */
    xf_list_for_each_entry(pos, &ch->head, xf_ps_subscr_t, node) {
        for (i = 0; i < XF_PS_SUBSCR_ID_LIST_NUM_MAX; i++) {
            if (pos->subscr_id_list[i] == id) {
                ++ref_cnt;
                /* NOTE 此处决定不能订阅两次相同事件 */
                break;
            }
        }
    }
    return ref_cnt;
}

xf_err_t xf_ps_update_event_ref_cnt(xf_ps_ch_t *const ch, xf_event_t *const e)
{
    int32_t ref_cnt;
    if ((ch == NULL) || (e == NULL)) {
        return XF_ERR_INVALID_ARG;
    }
    /* 统计当前通道中订阅该事件的订阅者数量 */
    ref_cnt = xf_ps_get_event_ref_cnt(ch, e->id);
    if (ref_cnt <= 0) {
        /* 无人订阅该事件 */
        XF_ERROR_LINE();
        XF_LOGD(TAG, "No one subscribed to event ID %u.", (unsigned int)e->id);
        return XF_FAIL;
    }
    e->ref_cnt = (xf_event_ref_cnt_t)ref_cnt;
    return XF_OK;
}

xf_err_t xf_ps_dispatch_ch(xf_ps_ch_t *const ch)
{
    xf_err_t xf_ret;
    xf_dq_size_t popped_size;
    xf_dq_size_t filled_size;
    xf_dq_size_t event_cnt;
    xf_event_t *e;
    uint32_t i;
    if (ch == NULL) {
        return XF_ERR_INVALID_ARG;
    }
    /* 处理事件 */
    filled_size = xf_deque_get_filled(&ch->event_queue);
    if (filled_size == 0) {
        goto l_no_event;
    }
    if (unlikely(!IS_ALIGNED(filled_size, XF_PS_ELEM_SIZE))) {
        /* 正常情况下都是 XF_PS_ELEM_SIZE 的整数倍 */
        XF_FATAL_ERROR();
    }
    event_cnt = filled_size / XF_PS_ELEM_SIZE;
    for (i = 0; i < event_cnt; i++) {
        popped_size = xf_deque_front_pop(&ch->event_queue,
                                         (void *)&e, XF_PS_ELEM_SIZE);
        if (unlikely(popped_size == 0)) {
            /* 说明有别处取走了，不算致命错误 */
            goto l_no_event;
        }
        if (unlikely((popped_size != XF_PS_ELEM_SIZE) || (e == NULL))) {
            XF_FATAL_ERROR();
        }
        if (unlikely(e->ref_cnt == 0)) {
            XF_LOGD(TAG, "no co subscribe event %u", e->id);
            xf_event_gc(e);
            continue;
        }
        xf_ret = xf_ps_notify(ch, e);
        if (xf_ret != XF_OK) {
            XF_ERROR_LINE();
        }
    }
l_no_event:;
    return XF_OK;
}

xf_ps_ch_t *xf_ps_get_default_ch(void)
{
    return sp_ch;
}

xf_err_t xf_ps_init(void)
{
    return xf_ps_init_ch(sp_ch, s_event_pool, sizeof(s_event_pool));
}

xf_ps_subscr_t *xf_ps_create_subscriber(xf_ps_subscr_cb_t cb_func, void *user_data)
{
    xf_ps_subscr_t *s;
    xf_err_t xf_ret;
    s = xf_ps_acquire_subscriber();
    if (s == NULL) {
        return NULL;
    }
    xf_ret = xf_ps_subscriber_init(s, cb_func, user_data);
    if (xf_ret != XF_OK) {
        goto l_err;
    }
    xf_ret = xf_ps_subscriber_attach(sp_ch, s);
    if (xf_ret != XF_OK) {
        goto l_err;
    }
    return s;
l_err:;
    xf_ps_release_subscriber(s);
    return NULL;
}

xf_err_t xf_ps_destroy_subscriber(xf_ps_subscr_t *s)
{
    xf_err_t xf_ret;
    if (s == NULL) {
        return XF_ERR_INVALID_ARG;
    }
    xf_ret = xf_ps_subscriber_detach(sp_ch, s);
    if (xf_ret != XF_OK) {
        goto l_err;
    }
    xf_ret = xf_ps_subscriber_deinit(s);
    if (xf_ret != XF_OK) {
        goto l_err;
    }
    xf_ret = xf_ps_release_subscriber(s);
l_err:;
    return xf_ret;
}

xf_err_t xf_ps_publish(xf_event_t *const e)
{
    return xf_ps_publish_to(sp_ch, e);
}

xf_err_t xf_ps_publish_immediately(xf_event_t *const e)
{
    return xf_ps_publish_immediately_to(sp_ch, e);
}

xf_err_t xf_ps_dispatch(void)
{
    return xf_ps_dispatch_ch(sp_ch);
}

/* ==================== [Static Functions] ================================== */

static xf_err_t xf_ps_notify(xf_ps_ch_t *const ch, xf_event_t *const e)
{
    xf_err_t xf_ret;
    xf_ps_subscr_t *pos;
    xf_ps_subscr_t *n;
    uint32_t i;
    if ((ch == NULL) || (e == NULL)) {
        return XF_ERR_INVALID_ARG;
    }
    /* 回调中可能删除订阅者对象 pos(回调中的 me), 必须使用 safe */
    xf_list_for_each_entry_safe(pos, n, &ch->head, xf_ps_subscr_t, node) {
        for (i = 0; i < XF_PS_SUBSCR_ID_LIST_NUM_MAX; i++) {
            if (pos->subscr_id_list[i] != e->id) {
                continue;
            }
            if (pos->cb_func) {
                pos->cb_func(pos, e);
            }
            if (sb_subscriber_created || sb_subscriber_deleted) {
                /* 
                    FIXME 是否需要处理订阅者？
                    此处未处理回调中订阅和取消订阅的情况，需要规范回调函数行为。
                    1.  回调中，不得创建新的订阅者对象。
                    2.  回调中，应该可以删除当前订阅对象 pos, 但是需要更新引用计数。
                        回调中，不得删除其他订阅者对象。
                 */
            }
            if (e->ref_cnt > 0) {
                --e->ref_cnt;
            }
            /* NOTE 由于不存在订阅者对象重复订阅同一个事件的情况，此处直接下一个 */
            break;
        }
        if (e->ref_cnt == 0) {
            break;
        }
    }
    if (e->ref_cnt == 0) {
        xf_ret = xf_event_gc(e);
    } else {
        /* TODO 此处强制回收是否合理？ */
        xf_ret = xf_event_gc_force(e);
    }
    return xf_ret;
}
