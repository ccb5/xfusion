/**
 * @file xf_task_internal.h
 * @author catcatBlue (catcatblue@qq.com)
 * @brief xf_task 内部接口，用户通常不需要直接调用。
 * @version 1.0
 * @date 2025-06-18
 *
 * SPDX-FileCopyrightText: 2025 CompanyNameMagicTag
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#ifndef __XF_TASK_INTERNAL_H__
#define __XF_TASK_INTERNAL_H__

/* ==================== [Includes] ========================================== */

#include "xf_task_def.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== [Defines] =========================================== */

/* ==================== [Typedefs] ========================================== */

/* ==================== [Global Prototypes] ================================= */

void xf_task_nest_depth_inc(void);
void xf_task_nest_depth_dec(void);
int8_t xf_task_get_nest_depth(void);

xf_task_t *xf_task_acquire(void);
xf_err_t xf_task_release(xf_task_t *task);

xf_err_t xf_task_init(xf_task_t *task, xf_task_cb_t cb_func, void *user_data);
xf_err_t xf_task_deinit(xf_task_t *task);

xf_err_t xf_task_acquire_timer(xf_task_t *me, xf_tick_t tick_period);
xf_err_t xf_task_release_timer(xf_task_t *me);

xf_err_t xf_task_acquire_subscr(xf_task_t *me, xf_event_id_t id);
xf_err_t xf_task_release_subscr(xf_task_t *me);

void xf_task_sched_timer_cb(xf_stimer_t *stimer);
void xf_resume_task_timer_cb(xf_stimer_t *stimer);
void xf_resume_task_subscr_cb(xf_subscr_t *s, uint8_t ref_cnt, void *arg);

xf_err_t xf_task_setup_wait_until(xf_task_t *me, xf_event_id_t id, xf_tick_t tick_period);
xf_err_t xf_task_teardown_wait_until(xf_task_t *me);

xf_task_t *xf_task_create_(xf_task_t *parent, xf_task_cb_t cb_func, void *user_data);
xf_err_t xf_task_destroy_(xf_task_t *task);

/* ==================== [Macros] ============================================ */

#define xf_task_create_i(_cb_func, _user_data) \
                                        xf_task_create_(NULL, xf_task_cb_cast(_cb_func), \
                                                        ((void *)(uintptr_t)(_user_data)))

#define xf_task_create_subtask_i(_me, _cb_func, _user_data) \
                                        xf_task_create_(xf_task_cast(_me), xf_task_cb_cast(_cb_func), \
                                                        ((void *)(uintptr_t)(_user_data)))

#define xf_task_destroy_i(_task)        xf_task_destroy_(xf_task_cast(_task))

#define xf_task_run_i(_task, _arg)      (xf_task_attr_set_state((_task), XF_TASK_READY), \
                                            xf_task_run_direct((_task), (_arg)))

#define xf_task_start_i(_task, _cb_func, _user_data, _arg) \
                                        ((_task) = xf_task_create_i((_cb_func), (_user_data)), \
                                            xf_task_run_i((_task), (_arg)))

#define xf_task_begin_i(_me)            { \
                                            xf_task_state_t __task_state = XF_TASK_READY; \
                                            xf_task_t *__task; \
                                            UNUSED(me); /*!< 仅用于规定参数名必须为 me */ \
                                            UNUSED(arg); /*!< 仅用于规定参数名必须为 arg */ \
                                            UNUSED(__task_state); /*!< 可能未使用 */ \
                                            UNUSED(__task); /*!< 可能未使用 */ \
                                            xf_task_nest_depth_inc(); \
                                            xf_task_lc_resume(xf_task_cast(_me)->lc) \

#define xf_task_end_i(_me)                  xf_task_lc_end(xf_task_cast(_me)->lc); \
                                            xf_task_lc_init(xf_task_cast(_me)->lc); \
                                            xf_task_attr_set_state((_me), XF_TASK_TERMINATED); \
                                            xf_task_destroy_i(xf_task_cast(_me)); \
                                            xf_task_nest_depth_dec(); \
                                            return XF_TASK_TERMINATED; \
                                        }

#define xf_task_restart_i(_me)          do { \
                                            xf_task_lc_init(xf_task_cast(_me)->lc); \
                                            xf_task_attr_set_state((_me), XF_TASK_READY); \
                                            xf_task_nest_depth_dec(); \
                                            return XF_TASK_READY; \
                                        } while (0)

#define xf_task_yield_i(_me)            do { \
                                            __task_state = XF_TASK_BLOCKED; \
                                            xf_task_lc_set(xf_task_cast(_me)->lc); \
                                            /* 此处临时阻塞一次 */ \
                                            if (__task_state == XF_TASK_BLOCKED) { \
                                                xf_task_nest_depth_dec(); \
                                                return XF_TASK_READY; \
                                            } \
                                        } while (0)

#define xf_task_block_i(_me)            do { \
                                            xf_task_attr_set_state((_me), XF_TASK_BLOCKED); \
                                            xf_task_lc_set(xf_task_cast(_me)->lc); \
                                            /* 外部必须将此任务设为 XF_TASK_READY 才能继续执行 */ \
                                            if (xf_task_attr_get_state(_me) != XF_TASK_READY) { \
                                                xf_task_nest_depth_dec(); \
                                                return XF_TASK_BLOCKED; \
                                            } \
                                        } while (0)

/* TODO goto 换为 continue? */
#define xf_task_wait_subtask_i(_me, _arg) \
                                        do { \
                                            while (1) { \
                                                __task = xf_task_id_to_task(xf_task_cast(_me)->id_child); \
                                                if (__task == NULL) { XF_FATAL_ERROR(); } \
                                                __task_state = xf_task_run_i((__task), (_arg)); \
                                                if (__task_state != XF_TASK_TERMINATED) { \
                                                    /* 复制子任务的状态，此时子任务可能是 READY 或 BLOCKED */ \
                                                    xf_task_attr_set_state((_me), __task_state); \
                                                    __task_state = XF_TASK_BLOCKED; /*!< 临时阻塞一次 */ \
                                                    xf_task_lc_set(xf_task_cast(_me)->lc); \
                                                    if (__task_state == XF_TASK_BLOCKED) { \
                                                        xf_task_nest_depth_dec(); \
                                                        /* 返回子任务状态 */ \
                                                        return xf_task_attr_get_state(_me); \
                                                    } \
                                                    continue; /*!< 回到 while (1) */ \
                                                } \
                                                break; \
                                            } \
                                            xf_task_cast(_me)->id_child = XF_TASK_ID_INVALID; \
                                        } while (0)

#define xf_task_await_i(_me, _cb_func, _user_data, _arg) \
                                        do { \
                                            xf_task_create_subtask_i((_me), (_cb_func), (_user_data)); \
                                            xf_task_wait_subtask_i((_me), (_arg)); \
                                        } while (0)

#define xf_task_delay_i(_me, _tick)     do { \
                                            xf_task_acquire_timer((_me), (_tick)); \
                                            xf_task_block_i((_me)); \
                                            xf_task_release_timer((_me)); \
                                        } while (0)

#define xf_task_delay_ms_i(_me, _ms)    xf_task_delay_i((_me), xf_tick_to_ms(_ms))

#define xf_task_wait_until_i(_me, _id, _tick, _xf_err) \
                                        do { \
                                            xf_task_setup_wait_until((_me), (_id), (_tick)); \
                                            xf_task_block_i((_me)); \
                                            if (xf_task_cast(_me)->id_subscr == XF_PS_ID_INVALID) { \
                                                /* 事件到达 */ \
                                                (_xf_err) = XF_OK; \
                                            } else { \
                                                /* 超时 */ \
                                                (_xf_err) = XF_ERR_TIMEOUT; \
                                            } \
                                            xf_task_teardown_wait_until(xf_task_cast(_me)); \
                                        } while (0)

#define xf_task_wait_until_ms_i(_me, _id, _ms, _xf_err) \
                                        xf_task_wait_until_i((_me), (_id), xf_tick_to_ms(_ms), (_xf_err))

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __XF_TASK_INTERNAL_H__ */
