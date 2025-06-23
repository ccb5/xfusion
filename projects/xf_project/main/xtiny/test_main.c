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
#define EXAMPLE_TASK_AWAIT              5
#define EXAMPLE_TASK_WAIT_EVENT         6
#define EXAMPLE_TASK_SCENE              7

#define EXAMPLE                         EXAMPLE_TASK_AWAIT

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
    XF_LOGI(tag, "s->event_id:  %u", (unsigned int)(uintptr_t)s->event_id);
    XF_LOGI(tag, "s->user_data: %u", (unsigned int)(uintptr_t)s->user_data);
    XF_LOGI(tag, "ref_cnt:      %u", (unsigned int)(uintptr_t)ref_cnt);
    XF_LOGI(tag, "arg:          %u", (unsigned int)(uintptr_t)arg);
}

void subscr_cb2(xf_subscr_t *s, uint8_t ref_cnt, void *arg)
{
    const char *const tag = "subscr_cb1";
    XF_LOGI(tag, "s->event_id:  %u", (unsigned int)(uintptr_t)s->event_id);
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
    xf_ps_subscr_t *subscr;
    xf_ps_subscr_id_t subscr_id;
    xf_ps_init();
    xf_subscribe(EVENT_ID_1, subscr_cb1, 0);
    subscr = xf_subscribe(EVENT_ID_2, subscr_cb1, 1);
    subscr_id = xf_ps_subscr_to_id(xf_subscribe(EVENT_ID_3, subscr_cb1, 2));
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
    /* 取消 subscr_cb2 订阅的 EVENT_ID_3 事件，且 user_data == 3 */
    xf_unsubscribe(EVENT_ID_3, subscr_cb2, 3);
    /* 忽略 user_data ，取消订阅订阅 EVENT_ID_3 的所有回调 */
    xf_unsubscribe(EVENT_ID_3, NULL, XF_PS_USER_DATA_INVALID);
    /* 通过订阅者对象取消订阅 */
    xf_unsubscribe_by_subscr(subscr);
    /* 通过订阅者对象 ID 取消订阅 */
    xf_unsubscribe_by_id(subscr_id);
    /* 忽略 user_data ，取消 subscr_cb1 订阅的所有事件 */
    xf_unsubscribe(XF_PS_SUBSCRIBER_NUM_MAX, subscr_cb1, XF_PS_USER_DATA_INVALID);
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

xf_task_async_t xf_task_1(xf_task_t *me, void *arg);

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

xf_task_async_t xf_task_1(xf_task_t *me, void *arg)
{
    const char *const tag = "xf_task_1";
    xf_task_begin(me);
    XF_LOGI(tag, "task%d begin", (int)xf_task_to_id(me));
    XF_LOGI(tag, "arg: %u", (unsigned int)(uintptr_t)arg);
    while (1) {
        xf_task_delay_ms(me, 1000);
        XF_LOGI(tag, "hi: %u", xf_tick_get_count());
    }
    XF_LOGI(tag, "task%d end", (int)xf_task_to_id(me));
    xf_task_end(me);
}

#elif EXAMPLE == EXAMPLE_TASK_WAIT_SUB

xf_task_async_t xf_task_1(xf_task_t *me, void *arg);
xf_task_async_t xf_task_2(xf_task_t *me, void *arg);
xf_task_async_t xf_task_21(xf_task_t *me, void *arg);

void test_main(void)
{
    xf_task_t *task;
    xf_tick_t delay_tick;
    xf_task_sched_init();

    /* 只创建，不调用 */
    xf_task_create(xf_task_2, NULL);
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

xf_task_async_t xf_task_1(xf_task_t *me, void *arg)
{
    const char *const tag = "xf_task_1";
    xf_task_begin(me);
    XF_LOGI(tag, "task%d begin", (int)xf_task_to_id(me));
    XF_LOGI(tag, "arg: %u", (unsigned int)(uintptr_t)arg);
    xf_task_delay_ms(me, 3000);
    while (1) {
        xf_task_delay_ms(me, 1000);
        XF_LOGI(tag, "hi: %u", xf_tick_get_count());
    }
    XF_LOGI(tag, "task%d end", (int)xf_task_to_id(me));
    xf_task_end(me);
}

xf_task_async_t xf_task_2(xf_task_t *me, void *arg)
{
    const char *const tag = "xf_task_2";
    xf_task_begin(me);
    XF_LOGI(tag, "task%d begin", (int)xf_task_to_id(me));
    XF_LOGI(tag, "arg: %u", (unsigned int)(uintptr_t)arg);
    xf_task_await(me, xf_task_21, NULL, arg);
    XF_LOGI(tag, "hi: %u", xf_tick_get_count());
    XF_LOGI(tag, "task%d end", (int)xf_task_to_id(me));
    xf_task_end(me);
}

xf_task_async_t xf_task_21(xf_task_t *me, void *arg)
{
    const char *const tag = "xf_task_21";
    xf_task_begin(me);
    XF_LOGI(tag, "task%d begin", (int)xf_task_to_id(me));
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
    XF_LOGI(tag, "task%d end", (int)xf_task_to_id(me));
    xf_task_end(me);
}

#elif EXAMPLE == EXAMPLE_TASK_AWAIT

xf_task_async_t xf_task_1(xf_task_t *me, void *arg);
xf_task_async_t xf_task_2(xf_task_t *me, void *arg);
xf_task_async_t xf_task_11(xf_task_t *me, void *arg);
xf_task_async_t xf_task_111(xf_task_t *me, void *arg);

void test_main(void)
{
    xf_task_t *task;
    xf_tick_t delay_tick;
    xf_task_sched_init();

    xf_task_start(task, xf_task_1, NULL, 12345678);

    while (1) {
        delay_tick = xf_stimer_handler();
        if (delay_tick != 0) {
            osDelayMs(delay_tick);
            (void)xf_tick_inc(delay_tick);
        }
    }
}

xf_task_async_t xf_task_1(xf_task_t *me, void *arg)
{
    const char *const tag = "xf_task_1";
    xf_task_async_t task_state;
    xf_task_t *task;

    xf_task_begin(me);

    XF_LOGI(tag, "task%d begin", (int)xf_task_to_id(me));
    XF_LOGI(tag, "arg: %u", (unsigned int)(uintptr_t)arg);

    XF_LOGI(tag, "start xf_task_2");
    task_state = xf_task_start(task, xf_task_2, NULL, arg);
    XF_LOGI(tag, "xf_task_2 task_state: %d", (int)task_state);

    XF_LOGI(tag, "start_subtask xf_task_11");
    xf_task_await(me, xf_task_11, NULL, arg);
    XF_LOGI(tag, "xf_task_11 will definitely terminate.");

    XF_LOGI(tag, "task%d end", (int)xf_task_to_id(me));
    xf_task_end(me);
}

xf_task_async_t xf_task_2(xf_task_t *me, void *arg)
{
    const char *const tag = "xf_task_2";
    xf_task_begin(me);
    XF_LOGI(tag, "task%d begin", (int)xf_task_to_id(me));
    XF_LOGI(tag, "arg: %u", (unsigned int)(uintptr_t)arg);
    while (1) {
        XF_LOGI(tag, "hi: %u", xf_tick_get_count());
        xf_task_delay_ms(me, 500);
    }
    XF_LOGI(tag, "task%d end", (int)xf_task_to_id(me));
    xf_task_end(me);
}

xf_task_async_t xf_task_11(xf_task_t *me, void *arg)
{
    const char *const tag = "xf_task_11";
    xf_task_begin(me);
    XF_LOGI(tag, "task%d begin", (int)xf_task_to_id(me));
    XF_LOGI(tag, "arg: %u", (unsigned int)(uintptr_t)arg);

    XF_LOGI(tag, "hi: %u", xf_tick_get_count());

    /* 嵌套 await */
    xf_task_await(me, xf_task_111, NULL, arg);

    XF_LOGI(tag, "task%d end", (int)xf_task_to_id(me));
    xf_task_end(me);
}

xf_task_async_t xf_task_111(xf_task_t *me, void *arg)
{
    const char *const tag = "xf_task_111";
    xf_task_begin(me);
    XF_LOGI(tag, "task%d begin", (int)xf_task_to_id(me));
    XF_LOGI(tag, "arg: %u", (unsigned int)(uintptr_t)arg);

    me->user_data = NULL;
    while (1) {
        XF_LOGI(tag, "hi: %u", xf_tick_get_count());
        xf_task_delay_ms(me, 500);
        me->user_data = (void *)((uintptr_t)me->user_data + 1);
        if (me->user_data == (void *)(uintptr_t)3) {
            break;
        }
    }

    XF_LOGI(tag, "task%d end", (int)xf_task_to_id(me));
    xf_task_end(me);
}

#elif EXAMPLE == EXAMPLE_TASK_WAIT_EVENT

xf_task_async_t xf_task_publish(xf_task_t *me, void *arg);
xf_task_async_t xf_task_subscribe(xf_task_t *me, void *arg);

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

xf_task_async_t xf_task_publish(xf_task_t *me, void *arg)
{
    const char *const tag = "xf_task_1";
    xf_err_t xf_ret;
    xf_tick_t delay_tick;
    xf_task_begin(me);
    XF_LOGI(tag, "task%d begin", (int)xf_task_to_id(me));
    XF_LOGI(tag, "arg: %u", (unsigned int)(uintptr_t)arg);
    me->user_data = NULL;
    while (1) {
        delay_tick = (ex_random() % 1500) + 1;
        xf_task_delay_ms(me, delay_tick);
        me->user_data = (void *)((uintptr_t)me->user_data + 1);
        xf_ret = xf_publish(EVENT_ID_1, me->user_data);
        XF_LOGI(tag, "publish: %u (%u), ret: %d",
                (unsigned int)(uintptr_t)me->user_data,
                (unsigned int)(uintptr_t)xf_tick_get_count(),
                (int)xf_ret);
    }
    XF_LOGI(tag, "task%d end", (int)xf_task_to_id(me));
    xf_task_end(me);
}

xf_task_async_t xf_task_subscribe(xf_task_t *me, void *arg)
{
    const char *const tag = "xf_task_2";
    xf_err_t xf_ret;
    xf_task_begin(me);
    XF_LOGI(tag, "task%d begin", (int)xf_task_to_id(me));
    XF_LOGI(tag, "arg: %u", (unsigned int)(uintptr_t)arg);
    while (1) {
        xf_task_wait_until_ms(me, EVENT_ID_1, 500, xf_ret);
        if (xf_ret == XF_ERR_TIMEOUT) {
            XF_LOGI(tag, "timeout. curr: %u", xf_tick_get_count());
        } else {
            XF_LOGI(tag, "got: %u", (unsigned int)(uintptr_t)arg);
        }
    }
    XF_LOGI(tag, "task%d end", (int)xf_task_to_id(me));
    xf_task_end(me);
}

#elif EXAMPLE == EXAMPLE_TASK_SCENE

#define EVENT_ID_1              123
#define EVENT_ID_UART_RECEIVE   345
#define EVENT_TIMEOUT           1000
#define LCD_RESET_IO            0

void _lv_timer_handler(void);
void _lv_init(void);
void _xf_gpio_set(int io, int value);
void _xf_uart_write(int uart_id, const char *buf, int len);

XF_SUBSCR_FUNC(subscr_cb1);

XF_TASK_FUNC(publish_task);
XF_TASK_FUNC(subscribe_task);
XF_TASK_FUNC(subtask);
XF_TASK_FUNC(lcd_init_task);
XF_TASK_FUNC(lv_task);
XF_TASK_FUNC(uart_task);

void test_main(void)
{
    xf_task_t *task;
    xf_tick_t delay_tick;
    xf_task_sched_init();
    xf_ps_init();

    xf_subscribe(EVENT_ID_1, subscr_cb1, 0);

    xf_task_start(task, lv_task, NULL, "lv_task");
    xf_task_start(task, subscribe_task, NULL, "subscribe_task");

    xf_task_create(publish_task, NULL);
    xf_task_create(uart_task, NULL);

    while (1) {
        xf_dispatch();
        delay_tick = xf_stimer_handler();
        if (delay_tick != 0) {
            osDelayMs(delay_tick);
            (void)xf_tick_inc(delay_tick);
        }
    }
}

void wait_at_response(void)
{
    // uart_read(1, buf);
    // parse_data(buf);
}

XF_TASK_FUNC(uart_task)
{
    const char *const tag = "uart_task";
    xf_err_t xf_ret;
    xf_task_begin(me);
    while (1) {
        _xf_uart_write(1, "AT+RESET\r\n", sizeof("AT+RESET\r\n"));
        xf_task_wait_until_ms(me, EVENT_ID_UART_RECEIVE, 5000, xf_ret);
        if (xf_ret == XF_ERR_TIMEOUT) {
            XF_LOGI(tag, "timeout");
        } else {
            wait_at_response();
        }
        // xf_task_waitevent(uart_receive, 5000);
    }
    xf_task_end(me);
}

XF_SUBSCR_FUNC(subscr_cb1)
{
    const char *const tag = "subscr_cb1";
    // XF_LOGI(tag, "s->event_id:  %u", (unsigned int)(uintptr_t)s->event_id);
    // XF_LOGI(tag, "s->user_data: %u", (unsigned int)(uintptr_t)s->user_data);
    // XF_LOGI(tag, "ref_cnt:      %u", (unsigned int)(uintptr_t)ref_cnt);
    XF_LOGI(tag, "arg:          %s", (char *)arg);
}

XF_TASK_FUNC(publish_task)
{
    const char *const tag = "publish_task";
    xf_err_t xf_ret;
    static uint8_t cnt = 0;
    xf_task_begin(me);
    while (1) {
        if (cnt & 0x01) {
            xf_ret = xf_publish(EVENT_ID_1, "hello");
        } else {
            xf_ret = xf_publish_sync(EVENT_ID_1, "world");
        }
        cnt++;
        if (xf_ret != XF_OK) {
            XF_LOGI(tag, "publish failed: %d", xf_ret);
        }
        xf_task_delay_ms(me, 3000);
    }
    xf_task_end(me);
}

XF_TASK_FUNC(subscribe_task)
{
    const char *const tag = "subscribe_task";
    xf_err_t xf_ret;
    xf_task_begin(me);
    while (1) {
        xf_task_wait_until_ms(me, EVENT_ID_1, EVENT_TIMEOUT, xf_ret);
        if (xf_ret == XF_ERR_TIMEOUT) {
            XF_LOGI(tag, "timeout");
        } else {
            XF_LOGI(tag, "got event");
            xf_task_await(me, subtask, NULL, arg);
        }
    }
    xf_task_end(me);
}

XF_TASK_FUNC(subtask)
{
    const char *const tag = "subtask";
    xf_task_begin(me);
    XF_LOGI(tag, "got: %s", (char *)arg);
    xf_task_delay(me, 1000);
    XF_LOGI(tag, "end");
    xf_task_end(me);
}

XF_TASK_FUNC(lcd_init_task)
{
    xf_task_begin(me);
    _xf_gpio_set(LCD_RESET_IO, 0);
    xf_task_delay(me, 120);
    _xf_gpio_set(LCD_RESET_IO, 1);
    xf_task_delay(me, 10);
    // cmd, data
    XF_LOGI("lcd_init_task", "lcd init done");
    xf_task_end(me);
}

XF_TASK_FUNC(lv_task)
{
    xf_task_begin(me);
    xf_task_await(me, lcd_init_task, NULL, arg);
    _lv_init();
    while (1) {
        _lv_timer_handler();
        xf_task_delay(me, 30);
    }
    xf_task_end(me);
}

void _lv_timer_handler(void)
{
    const char *const tag = "subtask";
    static uint8_t cnt = 0;
    cnt++;
    if (cnt == 100) {
        XF_LOGI(tag, "running");
        cnt = 0;
    }
    return;
}

void _lv_init(void)
{
    XF_LOGI("lv", "init");
}

void _xf_gpio_set(int io, int value)
{
    XF_LOGI("_xf_gpio_set", "io: %d, value: %d", io, value);
}

void _xf_uart_write(int uart_id, const char *buf, int len)
{
    XF_LOGI("_xf_uart_write", "uart_id: %d, buf: \"%s\", len: %d", uart_id, buf, len);
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
