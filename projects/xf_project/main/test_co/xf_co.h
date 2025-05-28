/**
 * @file xf_co.h
 * @author catcatBlue (catcatblue@qq.com)
 * @brief
 * @version 1.0
 * @date 2025-05-15
 *
 * SPDX-FileCopyrightText: 2025 CompanyNameMagicTag
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#ifndef __XF_CO_H__
#define __XF_CO_H__

/* ==================== [Includes] ========================================== */

#include "xf_utils.h"
#include "xf_event.h"

#include "xf_co_def.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== [Defines] =========================================== */

/* ==================== [Typedefs] ========================================== */

/* ==================== [Global Prototypes] ================================= */

/* ==================== [Macros] ============================================ */

#include <unistd.h>
#include <sys/time.h>
#include "xf_osal.h"

__STATIC_INLINE xf_co_timestamp_t xf_co_get_timestamp_freq(void)
{
    // return (1000U * 1000U);
    return osKernelGetTickFreq();
}

__STATIC_INLINE xf_co_timestamp_t _xf_co_get_tick(void)
{
    // struct timeval tv;
    // struct timezone tz;
    // gettimeofday(&tv, &tz);
    // return (xf_co_timestamp_t)(tv.tv_sec * 1000U * 1000U + tv.tv_usec);
    return osKernelGetTickCount();
}

__STATIC_INLINE xf_co_timestamp_t xf_co_tick_to_us(xf_co_timestamp_t tick)
{
    return tick / xf_co_get_timestamp_freq();
}

__STATIC_INLINE xf_co_timestamp_t xf_co_tick_to_ms(xf_co_timestamp_t tick)
{
    return tick / (xf_co_get_timestamp_freq() / 1000U);
}

__STATIC_INLINE xf_co_timestamp_t xf_co_us_to_tick(xf_co_timestamp_t us)
{
    return us * xf_co_get_timestamp_freq();
}

__STATIC_INLINE xf_co_timestamp_t xf_co_ms_to_tick(xf_co_timestamp_t ms)
{
    return ms * (xf_co_get_timestamp_freq() / 1000U);
}

__STATIC_INLINE xf_co_timestamp_t _xf_co_get_us(void)
{
    return (_xf_co_get_tick() * 1000U * 1000U) / xf_co_get_timestamp_freq();
}

__STATIC_INLINE xf_co_timestamp_t _xf_co_get_ms(void)
{
    return _xf_co_get_us() / 1000U;
}

#define xf_co_get_tick()            _xf_co_get_tick()
#define xf_co_get_us()              _xf_co_get_us()
#define xf_co_get_ms()              _xf_co_get_ms()

typedef xf_bitmap32_t xf_co_bitmap_t;

typedef struct xf_co_subscr_list {
    xf_co_bitmap_t co_bm[XF_BITMAP_GET_BLK_SIZE(XF_CO_NUM_MAX)];
} xf_co_subscr_list_t;

typedef uint8_t xf_co_tim_attr_t;
/* TODO 可能存在溢出问题 */
typedef uint32_t xf_co_tim_timestamp_t;

typedef struct xf_co_timer_event {
    xf_event_t              base;
    xf_co_tim_attr_t        attr;
#define XF_TIM_ATTR_ONESHOOT_B  (0x00)
    xf_co_tim_timestamp_t   ts_wakeup;
    xf_co_tim_timestamp_t   ts_interval;
} xf_co_tim_event_t;

#define xf_te_cast(_e)                  ((xf_co_tim_event_t *)(_e))

#define xf_co_tim_get_attr_oneshoot(_e) BIT_GET(xf_te_cast(_e)->attr, \
                                                XF_TIM_ATTR_ONESHOOT_B)
#define xf_co_tim_set_attr_oneshoot(_e, _value) \
                                        BIT_SET(xf_te_cast(_e)->attr, \
                                                XF_TIM_ATTR_ONESHOOT_B, \
                                                (_value))

__STATIC_INLINE xf_co_id_t xf_co_bm_blk_find_max(xf_co_bitmap_t bitmap_blk)
{
    STATIC_ASSERT(sizeof(xf_co_bitmap_t) <= sizeof(uint32_t));
    return (xf_co_id_t)xf_am_log2_u32((uint32_t)bitmap_blk);
}

xf_err_t xf_co_sched_init(void);
xf_err_t xf_co_sched_deinit(void);
xf_err_t xf_co_sched_run(xf_event_t **pp_e);

xf_err_t xf_co_create_(xf_co_func_t func, void *user_data, xf_co_t **pp_co);
#define xf_co_create(_func, _user_data) xf_co_create_(xf_co_func_cast(_func), \
                                                      ((void *)(uintptr_t)(_user_data)), NULL)

xf_err_t xf_co_destroy(xf_co_t *co);

#define XF_EVENT_ID_INVALID             ((xf_event_id_t)~(xf_event_id_t)0U)
/* 获取唯一事件 id */
xf_err_t xf_event_acquire_id_(xf_event_id_t *p_eid);
__STATIC_INLINE xf_event_id_t xf_event_acquire_id(void);
xf_err_t xf_event_release_id(xf_event_id_t eid);

xf_co_tim_event_t *xf_co_tim_event_acquire(void);
xf_err_t xf_co_tim_event_release(xf_co_tim_event_t *cte);

/* me 之后用于追溯发布事件的协程 */
xf_err_t xf_co_publish_(xf_co_t *const me, xf_event_t *const e);
#define xf_co_publish(_e) (xf_co_publish_(NULL, (xf_event_t *)(_e)))
xf_err_t xf_co_subscribe(xf_co_t *const me, xf_event_id_t id);
xf_err_t xf_co_unsubscribe(xf_co_t *const me, xf_event_id_t id);

// xf_err_t xf_co_sched_co_add(xf_co_t *const co);
// xf_err_t xf_co_sched_co_del(xf_co_t *const co);

// xf_err_t xf_co_sched_co_add_to(xf_co_t *const co_sched, xf_co_t *const co);
// xf_err_t xf_co_sched_co_del_from(xf_co_t *const co_sched, xf_co_t *const co);

xf_err_t xf_co_ctor(
    xf_co_t *const co, xf_co_func_t func, const xf_co_attr_t *p_attr);
xf_err_t xf_co_dtor(xf_co_t *const co);

#define xf_co_attr_default()            NULL

#define xf_co_init(_co, _func)          do { \
                                            xf_co_ctor((_co), xf_co_func_cast(_func), xf_co_attr_default()); \
                                        } while (0)

#define xf_co_begin(_me)                { \
                                            if (xf_co_get_flags_state(_me) == XF_CO_SUSPENDED) { \
                                                return XF_CO_SUSPENDED; \
                                            } \
                                            xf_co_lc_resume(xf_co_cast(_me)->lc) \

#define xf_co_end(_me)                      xf_co_lc_end(xf_co_cast(_me)->lc); \
                                            xf_co_lc_init(xf_co_cast(_me)->lc); \
                                            xf_co_set_flags_state((_me), XF_CO_TERMINATED); \
                                            xf_co_set_flags_terminate_bit((_me), 0); \
                                            return XF_CO_TERMINATED; \
                                        }

#define xf_co_call(_co, _e)             xf_co_cast(_co)->func(xf_co_cast(_co), (_e))
#define xf_co_call_explicit(_co_func, _co, _e) \
                                        xf_co_func_cast(_co_func)(xf_co_cast(_co), (_e))

#define xf_co_get_state(_co)            xf_co_get_flags_state(_co)

#define xf_co_set_ready(_co)            do { \
                                            xf_co_set_flags_state((_co), XF_CO_READY); \
                                        } while (0)

#define xf_co_set_block(_co)            do { \
                                            xf_co_set_flags_state((_co), XF_CO_BLOCKED); \
                                        } while (0)

#define xf_co_set_suspend(_co)          do { \
                                            xf_co_set_flags_state((_co), XF_CO_SUSPENDED); \
                                        } while (0)

#define xf_co_suspend(_me, _co)         do { \
                                            xf_co_set_suspend((_co)); \
                                            if ((_me) == (_co)) { \
                                                xf_co_lc_set(xf_co_cast(_me)->lc); \
                                                if (xf_co_get_flags_state(_me) == XF_CO_SUSPENDED) { \
                                                    return XF_CO_SUSPENDED; \
                                                } \
                                            } \
                                        } while (0)

#define xf_co_restart(_co)              do { \
                                            xf_co_lc_init(xf_co_cast(_co)->lc); \
                                            xf_co_set_flags_state((_co), XF_CO_READY); \
                                            return XF_CO_READY; \
                                        } while (0)

#define xf_co_exit(_me)                 do { \
                                            xf_co_lc_init(xf_co_cast(_me)->lc); \
                                            xf_co_set_flags_state((_me), XF_CO_TERMINATED); \
                                            /* 主动退出时不清除 terminate_bit */ \
                                            return XF_CO_TERMINATED; \
                                        } while (0)

#define xf_co_terminate(_me, _co)       do { \
                                            xf_co_set_flags_terminate_bit((_co), 1); \
                                            if ((_me) == (_co)) { \
                                                /* 终止自己时，通常认为已经释放了资源，因此无需从 END 分支退出 */ \
                                                xf_co_set_flags_terminate_bit((_co), 0); \
                                                xf_co_exit(_co); \
                                            } \
                                        } while (0)

#if 0
#define xf_co_wait_until(_me, _cond)    do { \
                                            xf_co_lc_set(xf_co_cast(_me)->lc); \
                                            if (!(_cond)) { \
                                                xf_co_set_flags_state((_me), XF_CO_READY); \
                                                return XF_CO_READY; \
                                            } \
                                        } while (0)

#define xf_co_wait_while(_me, _cond)    xf_co_wait_until((_me), !(_cond))
#endif

#define xf_co_yield(_me)                do { \
                                            xf_co_lc_set(xf_co_cast(_me)->lc); \
                                            if (xf_co_get_flags_state(_me) != XF_CO_READY) { \
                                                return XF_CO_BLOCKED; \
                                            } \
                                        } while (0)

#if 0
#define xf_co_yield_until(_me, _cond)   do { \
                                            __co_yield_flag = 0; \
                                            xf_co_lc_set(xf_co_cast(_me)->lc); \
                                            if ((__co_yield_flag == 0) || !(_cond)) { \
                                                xf_co_set_flags_state((_me), XF_CO_READY); \
                                                return XF_CO_READY; \
                                            } \
                                        } while (0)
#endif

/* TODO 标记恢复还是立即恢复？ */
#define xf_co_resume(_co)               xf_co_set_flags_state((_co), XF_CO_READY)

#define xf_co_delay(_me, _tick)         do { \
                                            xf_co_tim_event_t *__cte = xf_co_tim_event_acquire(); \
                                            if (!__cte) { /* TODO 判断返回值 */ return XF_CO_READY; }; \
                                            __cte->ts_wakeup = xf_co_get_tick() + (_tick); \
                                            xf_co_tim_set_attr_oneshoot(__cte, 1); \
                                            xf_co_subscribe(xf_co_cast(_me), __cte->base.id); \
                                            xf_co_set_block((_me)); \
                                            xf_co_yield((_me)); \
                                            xf_co_unsubscribe(xf_co_cast(_me), ((xf_co_tim_event_t *)e)->base.id); \
                                            xf_co_tim_event_release((xf_co_tim_event_t *)e); \
                                        } while (0)

#define xf_co_delay_ms(_me, _ms)        do { \
                                            xf_co_tim_event_t *__cte = xf_co_tim_event_acquire(); \
                                            if (!__cte) { /* TODO 判断返回值 */ return XF_CO_READY; }; \
                                            __cte->ts_wakeup = xf_co_get_tick() + xf_co_ms_to_tick(_ms); \
                                            xf_co_tim_set_attr_oneshoot(__cte, 1); \
                                            xf_co_subscribe(xf_co_cast(_me), __cte->base.id); \
                                            xf_co_set_block((_me)); \
                                            xf_co_yield((_me)); \
                                            xf_co_unsubscribe(xf_co_cast(_me), ((xf_co_tim_event_t *)e)->base.id); \
                                            xf_co_tim_event_release((xf_co_tim_event_t *)e); \
                                        } while (0)

/*
    TODO await 多个子协程
 */
#define xf_co_await(_me, _co_child_func, _co_child, _e) \
                                        do { \
                                            if (xf_co_get_attrs_priority(_me) == XF_CO_PRIORITY_NONE) { \
                                                xf_co_init(_co_child, _co_child_func); \
                                            } \
                                            /* 子协程没有终止时等待，直到子协程终止 */ \
                                            xf_co_set_flags_await_bit((_me), 1); \
                                            xf_co_wait_while( \
                                                (_me), \
                                                (((_co_child_func)((_co_child), (_e))) != XF_CO_TERMINATED) \
                                            ); \
                                            xf_co_set_flags_await_bit((_me), 0); \
                                        } while (0)

/* NOTE 需要手动初始化协程 */
#define xf_co_await_explicit(_me, _call_co_childs_func) \
                                        do { \
                                            /* 子协程没有终止时等待，直到子协程终止 */ \
                                            xf_co_set_flags_await_bit((_me), 1); \
                                            xf_co_wait_while( \
                                                (_me), \
                                                ((_call_co_childs_func) != XF_CO_TERMINATED) \
                                            ); \
                                            xf_co_set_flags_await_bit((_me), 0); \
                                        } while (0)

__STATIC_INLINE xf_event_id_t xf_event_acquire_id(void)
{
    xf_event_id_t eid;
    xf_err_t xf_ret;
    xf_ret = xf_event_acquire_id_(&eid);
    return (xf_ret == XF_OK) ? (eid) : (XF_EVENT_ID_INVALID);
}

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __XF_CO_H__ */
