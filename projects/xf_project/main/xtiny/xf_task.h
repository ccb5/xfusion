/**
 * @file xf_task.h
 * @author catcatBlue (catcatblue@qq.com)
 * @brief 
 * @version 1.0
 * @date 2025-06-16
 * 
 * SPDX-FileCopyrightText: 2025 CompanyNameMagicTag
 * SPDX-License-Identifier: Apache-2.0
 * 
 */

#ifndef __XF_TASK_H__
#define __XF_TASK_H__

/* ==================== [Includes] ========================================== */

#include "xf_task_def.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== [Defines] =========================================== */

/* ==================== [Typedefs] ========================================== */

/* ==================== [Global Prototypes] ================================= */

xf_err_t xf_task_sched_init(void);
xf_err_t xf_task_sched_deinit(void);

xf_task_t *xf_task_create_(xf_task_t *parent, xf_task_func_t func, void *user_data);
xf_err_t xf_task_destroy(xf_task_t *task);

xf_task_id_t xf_task_to_id(xf_task_t *s);
xf_task_t *xf_task_id_to_task(xf_task_id_t id);

/* ==================== [Macros] ============================================ */

#define xf_task_create(_func, _user_data) \
                                        xf_task_create_(NULL, xf_task_func_cast(_func), \
                                                        ((void *)(uintptr_t)(_user_data)))

#define xf_task_create_sub(_me, _func, _user_data) \
                                        xf_task_create_(xf_task_cast(_me), xf_task_func_cast(_func), \
                                                        ((void *)(uintptr_t)(_user_data)))

/* start = create + call */
#define xf_task_start(_func, _arg)      do { \
                                            xf_task_t *__task; \
                                            __task = xf_task_create((_func), (_arg)); \
                                            xf_task_resume_(NULL, __task, (_arg)); \
                                        } while (0)

#define xf_task_begin(_me)              { \
                                            xf_task_state_t __state; \
                                            xf_task_t *__task; \
                                            UNUSED(me); /*!< 仅用于规定参数名必须为 me */ \
                                            UNUSED(arg); /*!< 仅用于规定参数名必须为 arg */ \
                                            UNUSED(__state); /*!< 可能未使用 */ \
                                            UNUSED(__task); /*!< 可能未使用 */ \
                                            xf_task_lc_resume(xf_task_cast(_me)->lc) \

#define xf_task_end(_me)                    xf_task_lc_end(xf_task_cast(_me)->lc); \
                                            xf_task_lc_init(xf_task_cast(_me)->lc); \
                                            xf_task_attr_set_state((_me), XF_TASK_TERMINATED); \
                                            xf_task_destroy(xf_task_cast(_me)); \
                                            return XF_TASK_TERMINATED; \
                                        }

#define xf_task_get_state(_task)        xf_task_attr_get_state(_task)
#define xf_task_set_ready(_task)        xf_task_attr_set_state((_task), XF_TASK_READY)
#define xf_task_set_suspend(_task)      xf_task_attr_set_state((_task), XF_TASK_BLOCKED)

#define xf_task_restart(_task)          do { \
                                            xf_task_lc_init(xf_task_cast(_task)->lc); \
                                            xf_task_attr_set_state((_task), XF_TASK_READY); \
                                            return XF_TASK_READY; \
                                        } while (0)

#define xf_task_yield(_me)              do { \
                                            xf_task_attr_set_state((_me), XF_TASK_BLOCKED); \
                                            xf_task_lc_set(xf_task_cast(_me)->lc); \
                                            if (xf_task_attr_get_state(_me) == XF_TASK_BLOCKED) { \
                                                return XF_TASK_BLOCKED; \
                                            } \
                                        } while (0)

#define xf_task_resume_(_me, _task, _arg) \
                                        do { \
                                            UNUSED(_me); /*!< 之后还用于逻辑嵌套 */ \
                                            xf_task_nest_depth_inc(); \
                                            xf_task_attr_set_state((_task), XF_TASK_READY); \
                                            xf_task_call((_task), (_arg)); \
                                            xf_task_nest_depth_dec(); \
                                        } while (0)

#define xf_task_resume(_me, _task, _arg, _ret_state) \
                                        do { \
                                            UNUSED(_me); /*!< 之后还用于逻辑嵌套 */ \
                                            xf_task_nest_depth_inc(); \
                                            xf_task_attr_set_state((_task), XF_TASK_READY); \
                                            (_ret_state) = xf_task_call((_task), (_arg)); \
                                            xf_task_nest_depth_dec(); \
                                        } while (0)

#define xf_task_wait_subtask(_me, _arg) \
                                        do { \
                                            XCAT2(XF_TASK_SUB_LABEL, __LINE__):; \
                                            __task = xf_task_id_to_task(xf_task_cast(_me)->id_child); \
                                            if (__task == NULL) { XF_FATAL_ERROR(); } \
                                            xf_task_attr_set_state((__task), XF_TASK_READY); \
                                            xf_task_nest_depth_inc(); \
                                            __state = xf_task_call((__task), (_arg)); \
                                            xf_task_nest_depth_dec(); \
                                            if (__state != XF_TASK_TERMINATED) { \
                                                xf_task_yield((_me)); \
                                                goto XCAT2(XF_TASK_SUB_LABEL, __LINE__); \
                                            } \
                                            xf_task_cast(_me)->id_child = XF_TASK_ID_INVALID; \
                                        } while (0)

#define xf_task_start_sub(_me, _func, _arg) \
                                        do { \
                                            xf_task_create_sub((_me), (_func), (_arg)); \
                                            xf_task_wait_subtask((_me), (_arg)); \
                                        } while (0)

#define xf_task_delay(_me, _tick)       do { \
                                            xf_task_acquire_timer((_me), (_tick)); \
                                            xf_task_yield((_me)); \
                                            xf_task_release_timer((_me)); \
                                        } while (0)

#define xf_task_delay_ms(_me, _ms)      xf_task_delay((_me), xf_tick_to_ms(_ms))

#define xf_task_wait_until(_me, _id, _tick, _e_out, _xf_err) \
                                        do { \
                                            (_xf_err) = xf_task_setup_wait_until_1((_me), (_id), (_tick)); \
                                            if ((_xf_err) != XF_OK) { \
                                                /* 错误 */ \
                                                break; \
                                            } \
                                            xf_task_yield((_me)); \
                                            if (arg == NULL) { \
                                                /* 意外唤醒 */ \
                                                xf_task_attr_set_state((_me), XF_TASK_BLOCKED); \
                                                /* 使用前一个 xf_task_yield */ \
                                                return XF_TASK_BLOCKED; \
                                            } \
                                            if (arg == (void *)(xf_task_cast(_me)->t)) { \
                                                /* 超时 */ \
                                                (_xf_err) = XF_ERR_TIMEOUT; \
                                            } else { \
                                                /* 事件到达 */ \
                                                (_xf_err) = XF_OK; \
                                                (_e_out) = xf_event_cast(arg); \
                                            } \
                                            xf_task_teardown_wait_until(xf_task_cast(_me)); \
                                        } while (0)

#define xf_task_wait_until_ms(_me, _id, _tick, _e_out, _xf_err) \
                                        xf_task_wait_until((_me), (_id), xf_tick_to_ms(_tick), _e_out, _xf_err)

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __XF_TASK_H__ */
