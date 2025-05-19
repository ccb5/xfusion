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
    // return (1000 * 1000);
    return osKernelGetTickFreq();
}

__STATIC_INLINE xf_co_timestamp_t _xf_co_get_tick(void)
{
    // struct timeval tv;
    // struct timezone tz;
    // gettimeofday(&tv, &tz);
    // return (xf_co_timestamp_t)(tv.tv_sec * 1000 * 1000 + tv.tv_usec);
    return osKernelGetTickCount();
}

__STATIC_INLINE xf_co_timestamp_t xf_co_tick_to_us(xf_co_timestamp_t tick)
{
    return tick / xf_co_get_timestamp_freq();
}

__STATIC_INLINE xf_co_timestamp_t xf_co_tick_to_ms(xf_co_timestamp_t tick)
{
    return tick / (xf_co_get_timestamp_freq() / 1000);
}

__STATIC_INLINE xf_co_timestamp_t xf_co_us_to_tick(xf_co_timestamp_t us)
{
    return us * xf_co_get_timestamp_freq();
}

__STATIC_INLINE xf_co_timestamp_t xf_co_ms_to_tick(xf_co_timestamp_t ms)
{
    return ms * (xf_co_get_timestamp_freq() / 1000);
}

__STATIC_INLINE xf_co_timestamp_t _xf_co_get_us(void)
{
    return (_xf_co_get_tick() * 1000 * 1000) / xf_co_get_timestamp_freq();
}

__STATIC_INLINE xf_co_timestamp_t _xf_co_get_ms(void)
{
    return _xf_co_get_us() / 1000;
}

#define xf_co_get_tick()            _xf_co_get_tick()
#define xf_co_get_us()              _xf_co_get_us()
#define xf_co_get_ms()              _xf_co_get_ms()

xf_err_t xf_co_sched_co_add(xf_co_t *const co);
xf_err_t xf_co_sched_co_del(xf_co_t *const co);

xf_err_t xf_co_sched_co_add_to(xf_co_t *const co_sched, xf_co_t *const co);
xf_err_t xf_co_sched_co_del_from(xf_co_t *const co_sched, xf_co_t *const co);

xf_err_t xf_co_ctor(
    xf_co_t *const co, xf_co_func_t func, const xf_co_attr_t *attr);
xf_err_t xf_co_dtor(xf_co_t *const co);

#define xf_co_attr_default()            NULL

#define xf_co_init(_co, _func)          do { \
                                            xf_co_ctor((_co), (xf_co_func_t)(_func), xf_co_attr_default()); \
                                        } while (0)

#define xf_co_begin(_me)                { \
                                            char XF_CO_YIELD_FLAG = 1; \
                                            UNUSED(XF_CO_YIELD_FLAG); /*!< 只是为了防止警告 */ \
                                            if (xf_co_get_flags_state(_me) == XF_CO_SUSPENDED) { \
                                                return XF_CO_SUSPENDED; \
                                            } \
                                            xf_co_lc_resume(xf_co_cast(_me)->lc); \
                                            do { \
                                                xf_co_set_flags_state((_me), XF_CO_RUNNING); \
                                            } while (0)

#define xf_co_end(_me)                      xf_co_lc_end(xf_co_cast(_me)->lc); \
                                            XF_CO_YIELD_FLAG = 0; \
                                            xf_co_lc_init(xf_co_cast(_me)->lc); \
                                            xf_co_set_flags_state((_me), XF_CO_TERMINATED); \
                                            xf_co_set_flags_terminate_bit((_me), 0); \
                                            return XF_CO_TERMINATED; \
                                        }

#define xf_co_get_state(_co)            xf_co_get_flags_state(_co)

#define xf_co_yield(_me)                xf_co_yield_until((_me), 1)

#define xf_co_yield_until(_me, _cond)   do { \
                                            XF_CO_YIELD_FLAG = 0; \
                                            xf_co_lc_set(xf_co_cast(_me)->lc); \
                                            if ((XF_CO_YIELD_FLAG == 0) || !(_cond)) { \
                                                xf_co_set_flags_state((_me), XF_CO_WAITING); \
                                                return XF_CO_WAITING; \
                                            } \
                                            xf_co_set_flags_state((_me), XF_CO_RUNNING); \
                                        } while (0)

#define xf_co_suspend(_me, _co)         do { \
                                            xf_co_set_flags_state((_co), XF_CO_SUSPENDED); \
                                            if ((_me) == (_co)) { \
                                                xf_co_lc_set(xf_co_cast(_me)->lc); \
                                                if (xf_co_get_flags_state(_me) == XF_CO_SUSPENDED) { \
                                                    return XF_CO_SUSPENDED; \
                                                } \
                                                xf_co_set_flags_state((_me), XF_CO_RUNNING); \
                                            } \
                                        } while (0)

#define xf_co_resume(_co)               xf_co_set_flags_state((_co), XF_CO_WAITING)

#define xf_co_restart(_co)              do { \
                                            xf_co_lc_init(xf_co_cast(_co)->lc); \
                                            xf_co_set_flags_state((_co), XF_CO_WAITING); \
                                            return XF_CO_WAITING; \
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

#define xf_co_wait_until(_me, _cond)    do { \
                                            xf_co_lc_set(xf_co_cast(_me)->lc); \
                                            if (!(_cond)) { \
                                                xf_co_set_flags_state((_me), XF_CO_WAITING); \
                                                return XF_CO_WAITING; \
                                            } \
                                            xf_co_set_flags_state((_me), XF_CO_RUNNING); \
                                        } while (0)

#define xf_co_wait_while(_me, _cond)    xf_co_wait_until((_me), !(_cond))

#define xf_co_set_next_wakeup_tick(_co, _tick_inc) \
                                        do { \
                                            xf_co_cast(_co)->ts_wakeup = xf_co_get_tick() + (_tick_inc); \
                                        } while (0)

#define xf_co_delay(_me, _tick)         do { \
                                            xf_co_set_next_wakeup_tick((_me), (_tick)); \
                                            xf_co_wait_until((_me), xf_co_get_tick() >= xf_co_cast(_me)->ts_wakeup); \
                                        } while (0)

#define xf_co_delay_ms(_me, _ms)        do { \
                                            xf_co_set_next_wakeup_tick((_me), xf_co_ms_to_tick(_ms)); \
                                            xf_co_wait_until((_me), xf_co_get_tick() >= xf_co_cast(_me)->ts_wakeup); \
                                        } while (0)

#define xf_co_delay_until(_me, _tick)   do { \
                                            xf_co_cast(_me)->ts_wakeup = (_tick); \
                                            xf_co_wait_until( \
                                                (_me), \
                                                (xf_co_get_tick() >= xf_co_cast(_me)->ts_wakeup) \
                                            ); \
                                        } while (0)

/*
#define xf_co_schedule(_f)              ((_f) == XF_CO_WAITING)

#define xf_co_wait_thread(_me, _thread) xf_co_wait_while((_me), xf_co_schedule(_thread))

#define xf_co_spawn(_co, _co_child, _call_co_child_func) \
                                        do { \
                                            xf_co_init(_co_child); \
                                            xf_co_wait_thread((_co), (_call_co_child_func)); \
                                        } while (0)
 */

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

#define xf_co_mutex_init(_mutex)        ((_mutex)->flag = XF_CO_MUTEX_UNLOCKED)
#define xf_co_mutex_destroy(_mutex)     ((_mutex)->flag = XF_CO_MUTEX_UNLOCKED)
#define xf_co_mutex_is_locked(_mutex)   ((_mutex)->flag == XF_CO_MUTEX_LOCKED)

xf_err_t xf_co_mutex_trylock(xf_co_t *const me, xf_co_mutex_t *const mutex);
xf_err_t xf_co_mutex_unlock(xf_co_t *const me, xf_co_mutex_t *const mutex);

#define xf_co_mutex_acquire(_me, _mutex, _timeout, _res) \
                                        do { \
                                            (_res) = xf_co_mutex_trylock((_me), (_mutex)); \
                                            if (((_res) == XF_OK) || ((_timeout) == 0)) { \
                                                break; \
                                            } \
                                            /* FIXME 区分 系统 tick 和 时基 tick */ \
                                            /* 失败且超时不为 0，重试 */ \
                                            xf_co_cast(_me)->ts_wakeup = xf_co_get_tick(); \
                                            xf_co_cast(_me)->ts_timeout = xf_co_cast(_me)->ts_wakeup + (_timeout); \
                                            xf_co_cast(_me)->ts_wakeup += XF_CO_MIN_INTERVAL; \
                                            xf_co_set_flags_state((_me), XF_CO_WAITING); \
                                            \
                                            xf_co_lc_set(xf_co_cast(_me)->lc); \
                                            \
                                            if (xf_co_get_tick() < xf_co_cast(_me)->ts_wakeup) { \
                                                return XF_CO_WAITING; \
                                            } \
                                            (_res) = xf_co_mutex_trylock((_me), (_mutex)); \
                                            xf_co_cast(_me)->ts_wakeup += XF_CO_MIN_INTERVAL; \
                                            if (((_res) != XF_OK) \
                                                    && (xf_co_cast(_me)->ts_wakeup < xf_co_cast(_me)->ts_timeout)) { \
                                                return XF_CO_WAITING; \
                                            } \
                                            xf_co_set_flags_state((_me), XF_CO_RUNNING); \
                                        } while (0)

#define xf_co_mutex_release(_me, _mutex, _res) \
                                        do { \
                                            (_res) = xf_co_mutex_unlock((_me), (_mutex)); \
                                        } while (0)

#if CONFIG_XF_CO_ENABLE_MUTEX_OWNER
__STATIC_INLINE xf_co_t *xf_co_mutex_get_owner(xf_co_mutex_t *mutex)
{
    return mutex->owner;
}
#endif

#define xf_co_semaphore_init(_sem, _count_max, _count_initial) \
                                        do { \
                                            (_sem)->count_max = (_count_max); \
                                            (_sem)->count = (_count_initial); \
                                        } while (0)

#define xf_co_semaphore_destroy(_sem)   do { \
                                            xf_co_semaphore_init((_sem), 0, 0); \
                                        } while (0)

#define xf_co_semaphore_acquire(_me, _sem, _timeout, _res) \
                                        do { \
                                            if ((_sem)->count > 0) { \
                                                --(_sem)->count; \
                                                (_res) = XF_OK; \
                                                break; \
                                            } \
                                            if ((_timeout) == 0) { \
                                                (_res) = XF_FAIL; \
                                                break; \
                                            } \
                                            /* 失败且超时不为 0，重试 */ \
                                            xf_co_cast(_me)->ts_wakeup = xf_co_get_tick(); \
                                            xf_co_cast(_me)->ts_timeout = xf_co_cast(_me)->ts_wakeup + (_timeout); \
                                            xf_co_cast(_me)->ts_wakeup += XF_CO_MIN_INTERVAL; \
                                            xf_co_set_flags_state((_me), XF_CO_WAITING); \
                                            \
                                            xf_co_lc_set(xf_co_cast(_me)->lc); \
                                            \
                                            if (xf_co_get_tick() < xf_co_cast(_me)->ts_wakeup) { \
                                                return XF_CO_WAITING; \
                                            } \
                                            if ((_sem)->count > 0) { \
                                                --(_sem)->count; \
                                                (_res) = XF_OK; \
                                                xf_co_set_flags_state((_me), XF_CO_RUNNING); \
                                                break; \
                                            } \
                                            xf_co_cast(_me)->ts_wakeup += XF_CO_MIN_INTERVAL; \
                                            if (xf_co_cast(_me)->ts_wakeup < xf_co_cast(_me)->ts_timeout) { \
                                                return XF_CO_WAITING; \
                                            } \
                                            (_res) = XF_FAIL; \
                                            xf_co_set_flags_state((_me), XF_CO_RUNNING); \
                                        } while (0)

#define xf_co_semaphore_release(_me, _sem, _res) \
                                        do { \
                                            if ((_sem)->count < (_sem)->count_max) { \
                                                ++(_sem)->count; \
                                                (_res) = XF_OK; \
                                                break; \
                                            } \
                                            (_res) = XF_FAIL; \
                                        } while (0)

#define xf_co_semaphore_get_count(_sem) \
                                        ((_sem)->count)

#define xf_co_semaphore_get_count_max(_sem) \
                                        ((_sem)->count_max)

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __XF_CO_H__ */
