/**
 * @file xf_stimer.h
 * @author catcatBlue (catcatblue@qq.com)
 * @brief 软件定时器。
 * @version 1.0
 * @date 2025-06-04
 *
 * SPDX-FileCopyrightText: 2025 CompanyNameMagicTag
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#ifndef __XF_STIMER_H__
#define __XF_STIMER_H__

/* ==================== [Includes] ========================================== */

#include "xf_utils.h"
#include "xf_tick.h"
#include "xf_event.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== [Defines] =========================================== */

#define XF_STIMER_NUM_MAX               (16U)
#define XF_STIMER_INFINITY              (0xFFFFFFFFU)
#define XF_STIMER_NO_READY              (1000U)  /*!< 没有定时器时的延时间隔 */

/* ==================== [Typedefs] ========================================== */

typedef uint8_t xf_stimer_id_t;
#define XF_STIMER_ID_INVALID ((xf_stimer_id_t)~(xf_stimer_id_t)0) /*!< 无效定时器 ID */

/* 预声明 */
typedef struct xf_soft_timer xf_stimer_t;

typedef void (*xf_stimer_cb_t)(xf_stimer_t *stimer);

struct xf_soft_timer {
    xf_stimer_cb_t              cb_func;            /*!< 回调函数 */
    void                       *user_data;          /*!< 回调函数用户数据 */
    xf_tick_t                   tick_last_run;      /*!< 定时器开始时间/上一次运行时间，单位 tick */
    xf_tick_t                   tick_period;        /*!< 定时器周期，单位 tick */
};

/* ==================== [Global Prototypes] ================================= */

xf_stimer_t *xf_stimer_acquire(void);
xf_err_t xf_stimer_release(xf_stimer_t *stimer);

xf_err_t xf_stimer_init(xf_stimer_t *stimer);
xf_err_t xf_stimer_set_cb(xf_stimer_t *stimer, xf_stimer_cb_t cb_func);
xf_err_t xf_stimer_set_user_data(xf_stimer_t *stimer, void *user_data);
xf_err_t xf_stimer_set_period(xf_stimer_t *stimer, xf_tick_t tick_period);

xf_err_t xf_stimer_reset(xf_stimer_t *stimer);
xf_err_t xf_stimer_set_ready(xf_stimer_t *stimer);

xf_stimer_t *xf_stimer_create(
    xf_tick_t tick_period, xf_stimer_cb_t cb_func, void *user_data);
xf_err_t xf_stimer_destroy(xf_stimer_t *stimer);

xf_stimer_id_t xf_stimer_to_id(xf_stimer_t *s);
xf_stimer_t *xf_stimer_id_to_stimer(xf_stimer_id_t id);

xf_tick_t xf_stimer_handler(void);
/* 0~100 */
uint8_t xf_stimer_get_idle_percentage(void);

/* ==================== [Macros] ============================================ */

#define xf_stimer_cast(_stimer)                 ((xf_stimer_t *)(_stimer))

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __XF_STIMER_H__ */
