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

#include "xf_co_def.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== [Defines] =========================================== */

/* ==================== [Typedefs] ========================================== */

/* ==================== [Global Prototypes] ================================= */

extern xf_co_state_t xf_co_main(xf_co_t *const me, void *arg);

xf_err_t xf_co_top_init(void);
xf_err_t xf_co_top_run(void *arg);

xf_co_t *xf_co_create_(xf_co_func_t func, void *user_data, xf_co_cfg_t *p_cfg);
#define xf_co_create(_func, _user_data) xf_co_create_(xf_co_func_cast(_func), \
                                                      ((void *)(uintptr_t)(_user_data)), NULL)

xf_err_t xf_co_destroy(xf_co_t *co);

/* ==================== [Macros] ============================================ */

#define xf_co_begin(_me)                { \
                                            xf_event_t *__e; \
                                            UNUSED(__e); \
                                            UNUSED(me); /*!< 仅用于规定参数名必须为 me */ \
                                            UNUSED(arg); /*!< 仅用于规定参数名必须为 arg */ \
                                            xf_co_attr_set_state((_me), XF_CO_RUNNING); \
                                            xf_co_lc_resume(xf_co_cast(_me)->lc) \

#define xf_co_end(_me)                      xf_co_lc_end(xf_co_cast(_me)->lc); \
                                            xf_co_lc_init(xf_co_cast(_me)->lc); \
                                            xf_co_attr_set_state((_me), XF_CO_DEAD); \
                                            if (xf_co_attr_get_id(_me) < XF_CO_INTERNAL_NUM_MAX) { \
                                                xf_co_destroy(_me); \
                                            } \
                                            return XF_CO_DEAD; \
                                        }

#define xf_co_get_state(_co)            xf_co_attr_get_state(_co)

#define xf_co_set_normal(_co)           do { \
                                            xf_co_attr_set_state((_co), XF_CO_NORMAL); \
                                        } while (0)

#define xf_co_set_suspend(_co)          do { \
                                            xf_co_attr_set_state((_co), XF_CO_SUSPENDED); \
                                        } while (0)

#define xf_co_restart(_co)              do { \
                                            xf_co_lc_init(xf_co_cast(_co)->lc); \
                                            xf_co_attr_set_state((_co), XF_CO_SUSPENDED); \
                                            return XF_CO_SUSPENDED; \
                                        } while (0)

#define xf_co_yield(_me)                do { \
                                            UNUSED(__e); /*!< 必须在 begin end 内 */ \
                                            xf_co_attr_set_state((_me), XF_CO_SUSPENDED); \
                                            xf_co_lc_set(xf_co_cast(_me)->lc); \
                                            if (xf_co_attr_get_state(_me) == XF_CO_SUSPENDED) { \
                                                return XF_CO_SUSPENDED; \
                                            } \
                                            xf_co_attr_set_state((_me), XF_CO_RUNNING); \
                                        } while (0)

#define xf_co_resume(_me, _co, _arg, _res) \
                                        do { \
                                            UNUSED(_me); /*!< 之后还用于逻辑嵌套 */ \
                                            if ((xf_co_get_nest_depth() >= (XF_CO_NEST_DEPTH_MAX - 1)) \
                                                    || (!xf_co_cast(_co)->func)) { \
                                                XF_FATAL_ERROR(); \
                                            } \
                                            xf_co_nest_depth_inc(); \
                                            xf_co_attr_set_state((_me), XF_CO_NORMAL); \
                                            (_res) = xf_co_call((_co), (_arg)); \
                                            xf_co_attr_set_state((_me), XF_CO_RUNNING); \
                                            xf_co_nest_depth_dec(); \
                                        } while (0)

#define xf_co_delay(_me, _tick)         do { \
                                            xf_co_cast(_me)->temp.vd = NULL; \
                                            xf_stimer_create_oneshot((_tick), \
                                                                     (xf_stimer_cb_t)xf_stimer_call_co_cb, \
                                                                     (void *)(_me)); \
                                            xf_co_yield((_me)); \
                                            if ((!xf_co_cast(_me)->temp.stimer->user_data) || \
                                                    (xf_co_cast(_me)->temp.stimer->user_data != (_me))) { \
                                                /* 意外唤醒 */ \
                                                xf_co_attr_set_state((_me), XF_CO_SUSPENDED); \
                                                /* 使用前一个 xf_co_yield */ \
                                                return XF_CO_SUSPENDED; \
                                            } \
                                        } while (0)

#define xf_co_delay_ms(_me, _ms)        xf_co_delay((_me), xf_tick_to_ms(_ms))

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __XF_CO_H__ */
