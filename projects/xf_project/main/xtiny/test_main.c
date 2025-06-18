/**
 * @file test_co.c
 * @author catcatBlue (catcatblue@qq.com)
 * @brief
 * @version 1.0
 * @date 2025-05-15
 *
 * SPDX-FileCopyrightText: 2025 CompanyNameMagicTag
 * SPDX-License-Identifier: Apache-2.0
 *
 */

/* ==================== [Includes] ========================================== */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

#include "xf_common.h"
#include "xf_dstruct.h"
#include "xf_log.h"
#include "xf_algo.h"

#include "xf_osal.h"

#include <unistd.h>
#include <stdio.h>

#include "xf_ps.h"
#include "xf_stimer.h"
#include "xf_tick.h"
#include "xf_task.h"

/* ==================== [Defines] =========================================== */

#define TAG "test_main"

#define EXAMPLE_PS                      1
#define EXAMPLE_STIMER                  2
#define EXAMPLE_TASK_BASIC              3
#define EXAMPLE_TASK_WAIT_SUB           4
#define EXAMPLE_TASK_WAIT_EVENT         5

#define EXAMPLE                         EXAMPLE_TASK_WAIT_EVENT

/* ==================== [Typedefs] ========================================== */

/* ==================== [Static Prototypes] ================================= */

void ex_random_seed(uint32_t seed);
uint32_t ex_random(void);

/* ==================== [Static Variables] ================================== */

static uint32_t l_rnd = 42; // random seed

/* ==================== [Macros] ============================================ */

/* ==================== [Global Functions] ================================== */

#if EXAMPLE == EXAMPLE_PS

void subscr_cb1(xf_subscr_t *s, uint8_t ref_cnt, void *arg)
{
    const char *const tag = "subscr_cb1";
    XF_LOGI(tag, "s->id:        %u", (unsigned int)(uintptr_t)s->id);
    XF_LOGI(tag, "s->user_data: %u", (unsigned int)(uintptr_t)s->user_data);
    XF_LOGI(tag, "ref_cnt:      %u", (unsigned int)(uintptr_t)ref_cnt);
    XF_LOGI(tag, "arg:          %u", (unsigned int)(uintptr_t)arg);
}

void subscr_cb2(xf_subscr_t *s, uint8_t ref_cnt, void *arg)
{
    const char *const tag = "subscr_cb1";
    XF_LOGI(tag, "s->id:        %u", (unsigned int)(uintptr_t)s->id);
    XF_LOGI(tag, "s->user_data: %u", (unsigned int)(uintptr_t)s->user_data);
    XF_LOGI(tag, "ref_cnt:      %u", (unsigned int)(uintptr_t)ref_cnt);
    XF_LOGI(tag, "arg:          %u", (unsigned int)(uintptr_t)arg);
}

#define EVENT_ID_1  1
#define EVENT_ID_2  2
#define EVENT_ID_3  3

void test_main(void)
{
    uintptr_t cnt = 0;
    uint32_t cnt_while = 0;
    xf_ps_init();
    xf_subscribe(EVENT_ID_1, subscr_cb1, 0);
    xf_subscribe(EVENT_ID_2, subscr_cb1, 1);
    xf_subscribe(EVENT_ID_3, subscr_cb1, 2);
    xf_subscribe(EVENT_ID_3, subscr_cb2, 3);
    while (1) {
        xf_publish(EVENT_ID_1, cnt); cnt++;
        xf_publish(EVENT_ID_2, cnt); cnt++;
        xf_publish(EVENT_ID_3, cnt); cnt++;
        xf_publish_sync(EVENT_ID_3, cnt); cnt++;
        xf_dispatch();
        xf_log_printf("\r\n");
        osDelayMs(1000);
        cnt_while++;
        if (cnt_while >= 3) {
            break;
        }
    }
    /* 取消订阅 EVENT_ID_3 的所有回调 */
    xf_unsubscribe(EVENT_ID_3, NULL);
    /* 取消 subscr_cb1 订阅的所有事件 */
    xf_unsubscribe(XF_PS_SUBSCRIBER_NUM_MAX, subscr_cb1);
    /* 取消 subscr_cb2 订阅的 EVENT_ID_3 事件 */
    xf_unsubscribe(EVENT_ID_3, subscr_cb2);
}

#elif EXAMPLE == EXAMPLE_STIMER

static uint8_t repeat_count = 0;

void hi_timer_cb(xf_stimer_t *t)
{
    XF_LOGI(TAG, "hi: %u", xf_tick_get_count());
    XF_LOGI(TAG, "idle_percentage: %u", xf_stimer_get_idle_percentage());
    repeat_count++;
    if (repeat_count >= 10) {
        xf_stimer_destroy(t);
    }
}

void oneshot_timer_cb(xf_stimer_t *t)
{
    XF_LOGI(TAG, "oneshot_timer_cb: %u", xf_tick_get_count());
    xf_stimer_destroy(t);
}

void test_main(void)
{
    xf_tick_t delay_tick;
    UNUSED(delay_tick);
    xf_stimer_create(100U, (xf_stimer_cb_t)hi_timer_cb, NULL);
    xf_stimer_create(2000U, (xf_stimer_cb_t)oneshot_timer_cb, NULL);
    while (1) {
        delay_tick = xf_stimer_handler();
        osDelayMs(delay_tick);
        (void)xf_tick_inc(delay_tick);
    }
}

#elif EXAMPLE == EXAMPLE_TASK_BASIC

xf_task_state_t xf_task_1(xf_task_t *me, void *arg);

void test_main(void)
{
    xf_task_t *task;
    xf_tick_t delay_tick;
    xf_task_sched_init();

    /* 创建并调用 */
    xf_task_start(task, xf_task_1, NULL, 12345678U);

    while (1) {
        delay_tick = xf_stimer_handler();
        if (delay_tick != 0) {
            osDelayMs(delay_tick);
            (void)xf_tick_inc(delay_tick);
        }
    }
}

xf_task_state_t xf_task_1(xf_task_t *me, void *arg)
{
    const char *const tag = "xf_task_1";
    xf_task_begin(me);
    XF_LOGI(tag, "co%d begin", (int)xf_task_to_id(me));
    XF_LOGI(tag, "arg: %u", (unsigned int)(uintptr_t)arg);
    while (1) {
        xf_task_delay_ms(me, 1000);
        XF_LOGI(tag, "hi: %u", xf_tick_get_count());
    }
    XF_LOGI(tag, "co%d end", (int)xf_task_to_id(me));
    xf_task_end(me);
}

#elif EXAMPLE == EXAMPLE_TASK_WAIT_SUB

xf_task_state_t xf_task_1(xf_task_t *const me, void *arg);
xf_task_state_t xf_task_2(xf_task_t *const me, void *arg);
xf_task_state_t xf_task_21(xf_task_t *const me, void *arg);

void test_main(void)
{
    xf_task_t *task;
    xf_tick_t delay_tick;
    xf_task_sched_init();

    /* 创建并调用 */
    xf_task_start(task, xf_task_1, NULL, 12345678U);
    /* 只创建，不调用 */
    xf_task_create(xf_task_2, NULL);

    while (1) {
        delay_tick = xf_stimer_handler();
        if (delay_tick != 0) {
            osDelayMs(delay_tick);
            (void)xf_tick_inc(delay_tick);
        }
    }
}

xf_task_state_t xf_task_1(xf_task_t *const me, void *arg)
{
    const char *const tag = "xf_task_1";
    xf_task_begin(me);
    XF_LOGI(tag, "co%d begin", (int)xf_task_to_id(me));
    XF_LOGI(tag, "arg: %u", (unsigned int)(uintptr_t)arg);
    xf_task_delay_ms(me, 3000);
    while (1) {
        xf_task_delay_ms(me, 1000);
        XF_LOGI(tag, "hi: %u", xf_tick_get_count());
    }
    XF_LOGI(tag, "co%d end", (int)xf_task_to_id(me));
    xf_task_end(me);
}

xf_task_state_t xf_task_2(xf_task_t *const me, void *arg)
{
    const char *const tag = "xf_task_2";
    xf_task_begin(me);
    XF_LOGI(tag, "co%d begin", (int)xf_task_to_id(me));
    XF_LOGI(tag, "arg: %u", (unsigned int)(uintptr_t)arg);
    xf_task_start_subtask(me, xf_task_21, NULL, arg);
    XF_LOGI(tag, "hi: %u", xf_tick_get_count());
    XF_LOGI(tag, "co%d end", (int)xf_task_to_id(me));
    xf_task_end(me);
}

xf_task_state_t xf_task_21(xf_task_t *const me, void *arg)
{
    const char *const tag = "xf_task_21";
    xf_task_begin(me);
    XF_LOGI(tag, "co%d begin", (int)xf_task_to_id(me));
    XF_LOGI(tag, "arg: %u", (unsigned int)(uintptr_t)arg);
    me->user_data = NULL;
    while (1) {
        xf_task_delay_ms(me, 500);
        XF_LOGI(tag, "hi: %u", xf_tick_get_count());
        me->user_data = (void *)((uintptr_t)me->user_data + 1);
        if (me->user_data == (void *)(uintptr_t)3) {
            break;
        }
    }
    XF_LOGI(tag, "co%d end", (int)xf_task_to_id(me));
    xf_task_end(me);
}

#elif EXAMPLE == EXAMPLE_TASK_WAIT_EVENT

xf_task_state_t xf_task_publish(xf_task_t *const me, void *arg);
xf_task_state_t xf_task_subscribe(xf_task_t *const me, void *arg);

void test_main(void)
{
    xf_tick_t delay_tick;
    xf_task_sched_init();
    xf_ps_init();

    xf_task_create(xf_task_subscribe, NULL);
    xf_task_create(xf_task_publish, NULL);

    while (1) {
        xf_dispatch();
        delay_tick = xf_stimer_handler();
        if (delay_tick != 0) {
            osDelayMs(delay_tick);
            (void)xf_tick_inc(delay_tick);
        }
    }
}

#define EVENT_ID_1  1

xf_task_state_t xf_task_publish(xf_task_t *const me, void *arg)
{
    const char *const tag = "xf_task_1";
    xf_tick_t delay_tick;
    xf_task_begin(me);
    XF_LOGI(tag, "co%d begin", (int)xf_task_to_id(me));
    XF_LOGI(tag, "arg: %u", (unsigned int)(uintptr_t)arg);
    me->user_data = NULL;
    while (1) {
        delay_tick = (ex_random() % 1500) + 1;
        xf_task_delay_ms(me, delay_tick);
        me->user_data = (void *)((uintptr_t)me->user_data + 1);
        xf_publish(EVENT_ID_1, me->user_data);
        XF_LOGI(tag, "publish: %u (%u)",
                (unsigned int)(uintptr_t)me->user_data,
                (unsigned int)(uintptr_t)xf_tick_get_count());
    }
    XF_LOGI(tag, "co%d end", (int)xf_task_to_id(me));
    xf_task_end(me);
}

xf_task_state_t xf_task_subscribe(xf_task_t *const me, void *arg)
{
    const char *const tag = "xf_task_2";
    xf_err_t xf_ret;
    xf_task_begin(me);
    XF_LOGI(tag, "co%d begin", (int)xf_task_to_id(me));
    XF_LOGI(tag, "arg: %u", (unsigned int)(uintptr_t)arg);
    while (1) {
        xf_task_wait_until_ms(me, EVENT_ID_1, 500, xf_ret);
        if (xf_ret == XF_ERR_TIMEOUT) {
            XF_LOGI(tag, "timeout. curr: %u", xf_tick_get_count());
        } else {
            XF_LOGI(tag, "got: %u", (unsigned int)(uintptr_t)arg);
        }
    }
    XF_LOGI(tag, "co%d end", (int)xf_task_to_id(me));
    xf_task_end(me);
}

#endif

/* ==================== [Static Functions] ================================== */

void ex_random_seed(uint32_t seed)
{
    l_rnd = seed;
}

uint32_t ex_random(void)
{
    // "Super-Duper" Linear Congruential Generator (LCG)
    // LCG(2^32, 3*7*11*13*23, 0, seed)
    l_rnd = l_rnd * (3U * 7U * 11U * 13U * 23U);
    return l_rnd >> 8;
}
