/**
 * @file xf_co.c
 * @author catcatBlue (catcatblue@qq.com)
 * @brief
 * @version 1.0
 * @date 2025-05-19
 *
 * SPDX-FileCopyrightText: 2025 CompanyNameMagicTag
 * SPDX-License-Identifier: Apache-2.0
 *
 */

/* ==================== [Includes] ========================================== */

#include "xf_co.h"
#include "xf_algo.h"

/* ==================== [Defines] =========================================== */

#define TAG "xf_co"

/* ==================== [Typedefs] ========================================== */

/* ==================== [Static Prototypes] ================================= */

static xf_co_async_t xf_co_sched_func(xf_co_sched_t *const me, void *e);

/* ==================== [Static Variables] ================================== */

#if 0
/* TODO c89 初始化结构体 */
static xf_co_sched_t s_co_sched_ctx = {
    .base = {
        .lc             = XF_CO_LC_INIT_VALUE,
        .flags          = (XF_CO_READY)
        | ((0) << XF_CO_FLAGS_TERMINATE_BIT)    /*!< 不终止 */
        | ((0) << XF_CO_FLAGS_AWAIT_BIT),       /*!< 不等待 */
        .attrs          = (XF_CO_PRIORITY_SCHEDULER & XF_CO_ATTRS_PRIO_MASK)    /*!< 调度器优先级 */
        | ((XF_CO_TYPE_SCHEDULER) << XF_CO_ATTRS_TYPE_S),     /*!< 不设置类型 */
        .user_signal    = 0,
        .func           = (xf_co_func_t)xf_co_sched_func,
        .ts_wakeup      = 0,
        .ts_timeout     = 0,
        .node_w         = XF_INIT_LIST_HEAD(s_co_sched_ctx.base.node_w),
        // .node_d         = XF_INIT_LIST_HEAD(s_co_sched_ctx.base.node_d),
    },
    .head               = XF_INIT_LIST_HEAD(s_co_sched_ctx.head),
};
/* static  */xf_co_sched_t *sp_sched = &s_co_sched_ctx;
#endif

/* 协程池 */
static xf_co_t xf_co_pool[XF_CO_NUM_MAX] = {0};

static xf_dq_t xf_co_event_queue = {0};
static xf_dq_t *sp_eq = &xf_co_event_queue;
static xf_event_t *xf_event_pool[XF_CO_EVENT_NUM_MAX] = {0};
#define XF_EQ_ELEM_SIZE     (sizeof(xf_event_t *))

/* 定时器事件池 */
static xf_co_tim_event_t co_tim_event_pool[XF_CO_TIMER_NUM_MAX] = {0};
static xf_co_tim_event_t *sp_tep = co_tim_event_pool;

/* 唯一事件 id 位图 */
static xf_event_bitmap_t eid_bm[XF_BITMAP32_GET_BLK_SIZE(XF_CO_EVENT_NUM_MAX)];

/*
    协程订阅事件列表，
    索引为事件 id，订阅此事件的协程 id 以位图表示。
 */
static xf_co_subscr_list_t xf_co_subscr_list[XF_CO_EVENT_NUM_MAX] = {0};

/* ==================== [Macros] ============================================ */

/* ==================== [Global Functions] ================================== */

xf_err_t xf_co_ctor(
    xf_co_t *const co, xf_co_func_t func, const xf_co_attr_t *p_attr)
{
    if ((!co) || (!func)) {
        return XF_ERR_INVALID_ARG;
    }
    xf_co_lc_init(xf_co_cast(co)->lc);
    xf_co_cast(co)->func = func;
    xf_co_set_flags_state(co, XF_CO_READY);
    xf_co_set_flags_await_bit(co, 0);
    xf_co_set_flags_terminate_bit(co, 0);
    /* 不改变 xf_co_set_flags_reserved */
    if (!!p_attr) {
        xf_co_set_flags_type(co, p_attr->type);
        xf_co_set_flags_id(co, p_attr->id);
        /* TODO p_attr->co_sched */
#if CONFIG_XF_CO_ENABLE_USER_DATA
        xf_co_cast(co)->user_data = p_attr->user_data;
#endif
    } else {
        xf_co_set_flags_type(co, XF_CO_TYPE_NCTX);
        /* 不改变 id */
    }
    return XF_OK;
}

xf_err_t xf_co_dtor(xf_co_t *const co)
{
    if ((!co)) {
        return XF_ERR_INVALID_ARG;
    }
    const xf_co_t xf_co_empty = {0};
    *xf_co_cast(co) = xf_co_empty;
    return XF_OK;
}

xf_err_t xf_co_sched_init(void)
{
    xf_err_t xf_ret = XF_OK;
    xf_ret |= xf_deque_init(sp_eq, xf_event_pool, sizeof(xf_event_pool));
    return xf_ret;
}

xf_err_t xf_co_sched_deinit(void)
{
    return XF_OK;
}

xf_err_t xf_co_create_(xf_co_func_t func, void *user_data, xf_co_t **pp_co)
{
    int16_t i = 0;
    xf_err_t xf_ret;
    xf_co_attr_t attr = {0};
    attr.type = XF_CO_TYPE_NCTX;
    attr.user_data = user_data;
    /* 此处决定优先分配大的协程 id */
    /* TODO 优化此处 for */
    for (i = (int16_t)XF_CO_NUM_MAX - 1; i >= 0; i--) {
        if (NULL == xf_co_pool[i].func) {
            attr.id = i;
            xf_ret = xf_co_ctor(&xf_co_pool[i], func, &attr);
            if (xf_ret != XF_OK) {
                XF_LOGE(TAG, "ERR_LINE:%d", __LINE__);
                break;
            }
            xf_co_set_flags_id(&xf_co_pool[i], i);
            if (pp_co) {
                *pp_co = &xf_co_pool[i];
            }
            return XF_OK;
        }
    }
    return XF_FAIL;
}

xf_err_t xf_co_publish_(xf_co_t *const me, xf_event_t *const e)
{
    xf_dq_size_t dq_ret_size;
    int16_t i;
    xf_event_ref_cnt_t ref_cnt;
    UNUSED(me);
    if (e == NULL) {
        return XF_ERR_INVALID_ARG;
    }
    if (e->id >= XF_CO_EVENT_NUM_MAX) {
        return XF_FAIL;
    }
    /* 统计订阅该事件的协程数量 */
    ref_cnt = 0;
    for (i = 0; i < XF_BITMAP_GET_BLK_SIZE(XF_CO_NUM_MAX); i++) {
        /* TODO 更多事件的情况 */
        STATIC_ASSERT(sizeof(xf_co_subscr_list[e->id].co_bm[0]) <= sizeof(uint32_t));
        ref_cnt += (xf_event_ref_cnt_t)xf_popcount((uint32_t)xf_co_subscr_list[e->id].co_bm[i]);
    }
    e->ref_cnt = ref_cnt;
    if (ref_cnt == 0U) {
        XF_LOGD(TAG, "no co subscribe event %d", e->id);
        /* TODO 无人订阅的事件需要成功发布？ */
    }
    /* 加入事件队列， WARNING 此处仅拷贝指针 */
    dq_ret_size = xf_deque_back_push(sp_eq, (void *)&e, XF_EQ_ELEM_SIZE);
    if (dq_ret_size != XF_EQ_ELEM_SIZE) {
        return XF_FAIL;
    }
    return XF_OK;
}

xf_err_t xf_co_subscribe(xf_co_t *const me, xf_event_id_t id)
{
    xf_co_id_t co_id;
    if (id >= XF_CO_EVENT_NUM_MAX) {
        return XF_FAIL;
    }
    co_id = xf_co_get_flags_id(me);
    XF_BITMAP_SET1(xf_co_subscr_list[id].co_bm, co_id);
    return XF_OK;
}

xf_err_t xf_co_unsubscribe(xf_co_t *const me, xf_event_id_t id)
{
    xf_co_id_t co_id;
    if (id >= XF_CO_EVENT_NUM_MAX) {
        return XF_FAIL;
    }
    co_id = xf_co_get_flags_id(me);
    XF_BITMAP_SET0(xf_co_subscr_list[id].co_bm, co_id);
    return XF_OK;
}

xf_co_tim_event_t *xf_co_tim_acquire_oneshoot(xf_co_tim_timestamp_t ts_wakeup)
{
    uint16_t i = 0;
    for (i = 0; i < XF_CO_TIMER_NUM_MAX; i++) {
        if (sp_tep[i].ts_wakeup == 0U) {
            sp_tep[i].base.id = xf_event_acquire_id();
            sp_tep[i].ts_wakeup = ts_wakeup;
            xf_co_tim_set_attr_oneshoot(&sp_tep[i], 1);
            return &sp_tep[i];
        }
    }
    return NULL;
}

xf_err_t xf_co_tim_release(xf_co_tim_event_t *cte)
{
    xf_err_t xf_ret;
    uint16_t i = 0;
    if (cte == NULL) {
        return XF_ERR_INVALID_ARG;
    }
    for (i = 0; i < XF_CO_TIMER_NUM_MAX; i++) {
        if ((uintptr_t)&sp_tep[i] == (uintptr_t)cte) {
            xf_ret = xf_event_release_id(cte->base.id);
            cte->ts_wakeup = 0;
            cte->base.id = 0;
            cte->base.ref_cnt = 0;
            return xf_ret;
        }
    }
    return XF_FAIL;
}

xf_err_t xf_co_sched_run(xf_event_t **pp_e)
{
    int32_t i = 0;
    int32_t j = 0;
    int32_t k = 0;
    xf_co_t *co;
    xf_co_id_t co_id;
    xf_co_state_t co_state;
    xf_dq_size_t filled_size;
    xf_dq_size_t dq_ret_size;
    xf_dq_size_t event_cnt;
    xf_err_t xf_ret;
    xf_event_t *e;
    xf_event_t e_empty = {0};
    xf_co_bitmap_t co_bm_blk = 0; /*!< bitmap 中的一个块 */

    /* 处理事件 */
    filled_size = xf_deque_get_filled(sp_eq);
    if (filled_size == 0) {
        goto l_no_event;
    }
    /* 正常情况下都是 XF_EQ_ELEM_SIZE 的整数倍 */
    if (unlikely(((filled_size) & BIT_MASK(XF_EQ_ELEM_SIZE)) != 0U)) {
        XF_LOGE(TAG, "ERR_LINE:%d", __LINE__);
        xf_ret = XF_FAIL;
        goto l_err;
    }

    event_cnt = filled_size / XF_EQ_ELEM_SIZE;
    /* i: 事件索引 */
    for (i = 0; i < event_cnt; i++) {
        /* 获取事件，顺序 fifo */
        dq_ret_size = xf_deque_front_pop(sp_eq, &e, XF_EQ_ELEM_SIZE);
        if (unlikely(dq_ret_size == 0)) {
            XF_LOGE(TAG, "ERR_LINE:%d", __LINE__);
            xf_ret = XF_FAIL;
            goto l_err;
        }
        if (e->ref_cnt == 0U) {
            /*
                事件无人订阅
                TODO 重新加回事件循环？
             */
            XF_LOGD(TAG, "no co subscribe event %u", e->id);
            continue;
        }
        /*
            NOTE 此处决定 co_id 大的先处理事件
         */
        /* j: co bitmap 索引 */
        for (j = XF_BITMAP_GET_BLK_SIZE(XF_CO_NUM_MAX) - 1; j >= 0; j--) {
            co_bm_blk = xf_co_subscr_list[e->id].co_bm[j];
            if (0 == co_bm_blk) {
                continue;
            }

            /* k: 当前 co bitmap block 内的 co_id 索引 */
            k = xf_co_bm_blk_find_max(co_bm_blk);
            co_id = (xf_co_id_t)((uint32_t)j * (sizeof(xf_co_bitmap_t) * 8U) + (uint32_t)k);
            co = &xf_co_pool[co_id];
            if (unlikely(co->func == NULL)) {
                XF_LOGE(TAG, "ERR_LINE:%d", __LINE__);
                xf_ret = XF_FAIL;
                goto l_err;
            }
            xf_co_resume(co);
            co_state = xf_co_call(co, e);
            if (co_state == (xf_co_state_t)XF_CO_TERMINATED) {
                XF_LOGV(TAG, "co[%d] done.", (int)xf_co_get_flags_id(&xf_co_pool[i]));
                xf_co_dtor(&xf_co_pool[i]);
                /* TODO GC co */
            }

            BIT_SET0(co_bm_blk, k);
            if (0 == co_bm_blk) {
                continue;
            }

            if (e->ref_cnt == 0U) {
                /* TODO GC e */
            } else {
                /* TODO 事件未被处理完，重新加入事件队列？ */
            }
        }
    }

l_no_event:;

    /* 处理就绪 */
    for (i = 0; i < (int32_t)XF_CO_NUM_MAX; i++) {
        if ((xf_co_pool[i].func != NULL)
                && (xf_co_get_flags_state(&xf_co_pool[i]) == XF_CO_READY)
           ) {
            /* 就绪时无条件唤醒 */
            co = &xf_co_pool[i];
            co_state = xf_co_call(co, &e_empty);
            if (co_state == XF_CO_TERMINATED) {
                XF_LOGV(TAG, "co[%d] done.", (int)xf_co_get_flags_id(&xf_co_pool[i]));
                xf_co_dtor(&xf_co_pool[i]);
                /* TODO gc co */
            }
        }
    }

    /* 传出定时器事件 */

    if (pp_e) {
        *pp_e = NULL;
        /* 找最小时间 */
        /* j: 最小时间定时事件所在索引 */
        /* 先找首个有效定时器 */
        for (i = 0; i < (int32_t)XF_CO_TIMER_NUM_MAX; i++) {
            if (sp_tep[i].ts_wakeup != 0U) {
                break;
            }
        }
        if (i != (int32_t)XF_CO_TIMER_NUM_MAX) {
            k = i;
            for (j = i + 1; j < (int32_t)XF_CO_TIMER_NUM_MAX; j++) {
                if ((sp_tep[j].ts_wakeup != 0U)
                        && (sp_tep[j].ts_wakeup < sp_tep[k].ts_wakeup)) {
                    k = j;
                }
            }
            *pp_e = &co_tim_event_pool[k].base;
        }
    }

    xf_ret = XF_OK;
l_err:;
    return xf_ret;
}

xf_err_t xf_event_acquire_id_(xf_event_id_t *p_eid)
{
    int16_t j = 0;
    uint8_t k = 0;
    uint8_t eid_free_idx = 0;
    xf_event_bitmap_t bm_blk;
    if (p_eid == NULL) {
        return XF_FAIL;
    }
    /* 找首个为 0 的位 */
    for (j = ARRAY_SIZE(eid_bm) - 1; j >= 0; j--) {
        bm_blk = ~eid_bm[j];  /*!< 转为找首个为 1 的位 */
        if (bm_blk == 0) {
            continue;
        }
        k = (uint8_t)xf_log2(bm_blk);
        eid_free_idx = (uint8_t)j * ((uint8_t)sizeof(xf_event_bitmap_t) * 8U) + k;
        BIT_SET1(eid_bm[j], k);
        *p_eid = (xf_event_id_t)eid_free_idx;
        return XF_OK;
    }
    return XF_FAIL;
}

xf_err_t xf_event_release_id(xf_event_id_t eid)
{
    if (eid > (((xf_event_id_t)sizeof(eid_bm) * 8U) - 1U)) {
        return XF_FAIL;
    }
    if (XF_BITMAP_GET(eid_bm, eid) == 0) {
        return XF_FAIL;
    }
    XF_BITMAP_SET0(eid_bm, eid);
    return XF_OK;
}

/* ==================== [Static Functions] ================================== */
