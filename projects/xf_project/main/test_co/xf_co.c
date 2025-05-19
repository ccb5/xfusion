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

/* ==================== [Defines] =========================================== */

/* ==================== [Typedefs] ========================================== */

/* ==================== [Static Prototypes] ================================= */

static xf_co_async_t xf_co_sched_func(xf_co_sched_t *const me, void *e);

/* ==================== [Static Variables] ================================== */

/* TODO c89 初始化结构体 */
static xf_co_sched_t s_co_sched_ctx = {
    .base = {
        .lc             = XF_CO_LC_INIT_VALUE,
        .flags          = (XF_CO_READY & XF_CO_FLAGS_STATE_MASK)
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

/* ==================== [Macros] ============================================ */

/* ==================== [Global Functions] ================================== */

xf_err_t xf_co_ctor(
    xf_co_t *const co, xf_co_func_t func, const xf_co_attr_t *attr)
{
    /* TODO 检查参数 */
    xf_co_lc_init(xf_co_cast(co)->lc);
    xf_co_cast(co)->flags = XF_CO_WAITING; /* 注意，此处同时清除请求终止位 */
    xf_co_cast(co)->func = func;
    xf_list_init(&xf_co_cast(co)->node_w);
    if (attr) {
        xf_co_set_attrs_priority(co, attr->priority);
        xf_co_set_attrs_type(co, attr->type);
#if CONFIG_XF_CO_ENABLE_USER_DATA
        xf_co_cast(co)->user_data = attr->user_data;
#endif
    } else {
        xf_co_set_attrs_priority(co, XF_CO_PRIORITY_NORMAL);
        xf_co_set_attrs_type(co, XF_CO_TYPE_UNKNOWN);
        /* 忽略 attr->user_data */
    }
    return XF_OK;
}

xf_err_t xf_co_dtor(xf_co_t *const co)
{
    const xf_co_t xf_co_empty = {0};
    *xf_co_cast(co) = xf_co_empty;
    return XF_OK;
}

xf_err_t xf_co_sched_init(void)
{
    return XF_OK;
}

xf_err_t xf_co_sched_deinit(void)
{
    return XF_OK;
}

xf_err_t xf_co_sched_get_info(void)
{
    return XF_ERR_NOT_SUPPORTED;
}

xf_co_sched_state_t xf_co_sched_get_state(void)
{
    return 0;
}

xf_err_t xf_co_sched_co_add_to(xf_co_t *const me, xf_co_t *const co)
{
    xf_err_t xf_ret = XF_OK;
    xf_co_t *pos;
    xf_co_t *n;
    xf_co_t *next_co;
    xf_list_node_t *insert_pos = NULL;
    xf_co_priority_t prio_new;
    xf_co_priority_t prio_curr;
    xf_co_sched_t *co_sched;
    if (xf_co_get_attrs_type(me) != XF_CO_TYPE_SCHEDULER) {
        return XF_FAIL;
    }
    co_sched = (xf_co_sched_t *)me;
    prio_new = xf_co_get_attrs_priority(co);
    /*
        遍历链表，按优先级从大到小排序。
        如果新协程与已有协程优先级一致，则插到同优先级的最后面。
     */
    xf_list_for_each_entry_safe(pos, n, &co_sched->head, xf_co_t, node_w) {
        prio_curr = xf_co_get_attrs_priority(pos);
        if (prio_new > prio_curr) {
            /* 插到 pos 前面 */
            insert_pos = pos->node_w.prev;
            break;
        } else if (prio_new == prio_curr) {
            /*  跳过所有相同优先级的节点，插入到最后一个同优先级节点后 */
            next_co = xf_list_entry(pos->node_w.next, xf_co_t, node_w);
            while ((&next_co->node_w != &co_sched->head)
                    && (xf_co_get_attrs_priority(next_co) == prio_new)) {
                pos = next_co;
                next_co = xf_list_entry(pos->node_w.next, xf_co_t, node_w);
            }
            insert_pos = &pos->node_w;
            break;
        }
    }
    if (insert_pos) {
        /* 在 insert_pos 之后插入 */
        xf_list_add(&co->node_w, insert_pos);
    } else {
        /* 遍历完没找到，则放到链表尾部 */
        xf_list_add_tail(&co->node_w, &co_sched->head);
    }
    ++co_sched->count;
    return XF_OK;
}

xf_err_t xf_co_sched_co_add(xf_co_t *const co)
{
    return xf_co_sched_co_add_to(xf_co_cast(sp_sched), co);
}

xf_err_t xf_co_sched_co_del_from(xf_co_t *const me, xf_co_t *const co)
{
    xf_co_t *pos;
    xf_co_t *n;
    xf_co_sched_t *co_sched;
    if (xf_co_get_attrs_type(me) != XF_CO_TYPE_SCHEDULER) {
        return XF_FAIL;
    }
    co_sched = (xf_co_sched_t *)me;
    if (xf_list_empty(&co_sched->head)) {
        return XF_FAIL;
    }
    xf_list_for_each_entry_safe(pos, n, &co_sched->head, xf_co_t, node_w) {
        if (pos == co) {
            xf_list_del(&co->node_w);
            --co_sched->count;
            return XF_OK;
        }
    }
    return XF_FAIL;
}

xf_err_t xf_co_sched_co_del(xf_co_t *const co)
{
    return xf_co_sched_co_del_from(xf_co_cast(sp_sched), co);
}

xf_err_t xf_co_mutex_trylock(xf_co_t *const me, xf_co_mutex_t *const mutex)
{
    if (xf_co_mutex_is_locked(mutex)) {
        return XF_FAIL;
    }
    mutex->flag = XF_CO_MUTEX_LOCKED;
#if CONFIG_XF_CO_ENABLE_MUTEX_OWNER
    mutex->owner = xf_co_cast(me);
#endif
    return XF_OK;
}

xf_err_t xf_co_mutex_unlock(xf_co_t *const me, xf_co_mutex_t *const mutex)
{
#if CONFIG_XF_CO_ENABLE_MUTEX_OWNER
    if (mutex->owner != xf_co_cast(me)) {
        return XF_FAIL;
    }
    mutex->flag = XF_CO_MUTEX_UNLOCKED;
    mutex->owner = NULL;
    return XF_OK;
#endif
    UNUSED(me);
    mutex->flag = XF_CO_MUTEX_UNLOCKED;
    return XF_OK;
}

/* ==================== [Static Functions] ================================== */

typedef uint8_t xf_co_sched_event_type_t;
enum _xf_co_sched_event_type_t {
    XF_CO_SCHED_EVENT_DUMMY = 0x00,

    XF_CO_SCHED_EVENT_INIT,
    XF_CO_SCHED_EVENT_DEINIT,

    // XF_CO_SCHED_EVENT_GET_INFO,
    // XF_CO_SCHED_EVENT_GET_STATE,

    XF_CO_SCHED_EVENT_START,
    // XF_CO_SCHED_EVENT_LOCK,
    // XF_CO_SCHED_EVENT_UNLOCK,
    // XF_CO_SCHED_EVENT_RESTORE_LOCK,
    XF_CO_SCHED_EVENT_SUSPEND,
    XF_CO_SCHED_EVENT_RESUME,

    // XF_CO_SCHED_EVENT_SCHED,            /*!< 重新排序 */
    XF_CO_SCHED_EVENT_RUN,              /*!< 运行 */

    XF_CO_SCHED_EVENT_CO_ADD,           /*!< 新增一个协程 */
    XF_CO_SCHED_EVENT_CO_DEL,           /*!< 删除一个协程 */
    XF_CO_SCHED_EVENT_CO_TERMINATED,    /*!< 传出-有一个协程退出 */

    XF_CO_SCHED_EVENT_MAX,
};

typedef struct xf_co_sched_event_run {
    int dummy;
} xf_co_sched_event_run_t;

typedef struct xf_co_sched_event_resume {
    xf_co_timestamp_t                   ts_sleep; /*!< 系统处于睡眠或断电模式的时间。 */
} xf_co_sched_event_resume_t;

typedef struct xf_co_sched_event {
    xf_co_sched_event_type_t            type;
    union xf_co_sched_event_data {
        xf_co_sched_event_run_t         run;
    } u;
} xf_co_sched_event_t;

static xf_co_async_t xf_co_sched_func(xf_co_sched_t *const me, void *pe)
{
    xf_co_sched_t *const co_sched = me;
    xf_co_sched_event_t *e = (xf_co_sched_event_t *)pe;
    xf_co_t *pos;
    xf_co_t *n;
    xf_co_timestamp_t ts_wakeup_min = ~(xf_co_timestamp_t)0;
    xf_co_begin(me);
    while (1) {
        xf_list_for_each_entry_safe(pos, n, &co_sched->head, xf_co_t, node_w) {
            if (me == pos) {
                continue; /* 跳过调度器（自己） */
            }
            /* TODO 补偿 如果前一次睡眠，此时需要补偿 */

            if (xf_co_get_flags_await_bit(pos)) {
                /* TODO 如果此协程正等待子协程，则实际唤醒时间受子协程制约 */
            }
            if (pos->ts_wakeup > ts_wakeup_min) {
                ts_wakeup_min = pos->ts_wakeup;
            }
            if (xf_co_get_flags_state(pos)) {}
        }
    }
    xf_co_end(me);
}
