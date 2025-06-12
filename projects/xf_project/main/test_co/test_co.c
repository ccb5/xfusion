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

#include "test_co.h"
#include "xf_co.h"

/* ==================== [Defines] =========================================== */

static const char *const TAG = "test_co";

#define EXAMPLE_CO_BASIC                1
#define EXAMPLE_CO_DELAY                2
#define EXAMPLE_CO_WAIT_EVENT           3
#define EXAMPLE_STIMER                  4
#define EXAMPLE_PS                      5
#define EXAMPLE_PS_WITH_DATA            6

#define EXAMPLE                         EXAMPLE_PS_WITH_DATA

/* ==================== [Typedefs] ========================================== */

/* ==================== [Static Prototypes] ================================= */

void ex_random_seed(uint32_t seed);
uint32_t ex_random(void);

/* ==================== [Static Variables] ================================== */

static uint32_t l_rnd = 42; // random seed

/* ==================== [Macros] ============================================ */

/* ==================== [Global Functions] ================================== */

#if EXAMPLE == EXAMPLE_CO_BASIC

void test_main(void)
{
    xf_tick_t delay_tick;
    UNUSED(delay_tick);
    xf_co_top_init();
    while (1) {
        xf_co_top_run(NULL);
        delay_tick = xf_stimer_handler();
        osDelayMs(delay_tick);
        (void)xf_tick_inc(delay_tick);
    }
}

xf_co_state_t co_func(xf_co_t *const me, void *arg);

typedef struct xf_co_main_data {
    xf_co_t                *co[2];
} xf_co_main_data_t;

static xf_co_main_data_t xf_co_main_data = {0};

xf_co_state_t xf_co_main(xf_co_t *const me, void *arg)
{
    xf_co_state_t co_state;
    xf_co_main_data_t *sp_md = &xf_co_main_data;
    UNUSED(co_state);
    xf_co_begin(me);
    sp_md->co[0] = xf_co_create(co_func, 0);
    sp_md->co[1] = xf_co_create(co_func, 1);
    while (1) {
        xf_co_resume(me, sp_md->co[0], NULL, co_state);
        xf_co_resume(me, sp_md->co[1], NULL, co_state);
        xf_co_yield(me);
    }
    xf_co_end(me);
}

xf_co_state_t co_func(xf_co_t *const me, void *arg)
{
    xf_co_begin(me);
    printf("co%d begin\n", (int)(intptr_t)me->user_data);
    while (1) {
        XF_LOGI(TAG, "co%d curr: %8" PRIu32, (int)(intptr_t)me->user_data, xf_tick_get_count());
        xf_co_yield(me);
    }
    printf("co%d end\n", (int)(intptr_t)me->user_data);
    xf_co_end(me);
}

#elif EXAMPLE == EXAMPLE_CO_DELAY

void test_main(void)
{
    xf_tick_t delay_tick;
    UNUSED(delay_tick);
    xf_co_top_init();
    while (1) {
        xf_co_top_run(NULL);
        delay_tick = xf_stimer_handler();
        osDelayMs(delay_tick);
        (void)xf_tick_inc(delay_tick);
    }
}

xf_co_state_t co_func_0(xf_co_t *const me, void *arg);
xf_co_state_t co_func_1(xf_co_t *const me, void *arg);

xf_co_state_t xf_co_main(xf_co_t *const me, void *arg)
{
    xf_co_state_t co_state;
    xf_co_t *co;
    UNUSED(co_state);
    xf_co_begin(me);
    co = xf_co_create(co_func_0, 0);
    xf_co_resume(me, co, NULL, co_state);
    xf_co_end(me);
}

xf_co_state_t co_func_0(xf_co_t *const me, void *arg)
{
    xf_co_state_t co_state;
    xf_co_t *co;
    UNUSED(co_state);
    xf_co_begin(me);
    printf("co%d begin\n", (int)(intptr_t)me->user_data);
    co = xf_co_create(co_func_1, 1);
    xf_co_resume(me, co, NULL, co_state);
    while (1) {
        XF_LOGI(TAG, "co%d curr: %8" PRIu32, (int)(intptr_t)me->user_data, xf_tick_get_count());
        xf_co_delay_ms(me, 1000U);
    }
    printf("co%d end\n", (int)(intptr_t)me->user_data);
    xf_co_end(me);
}

xf_co_state_t co_func_1(xf_co_t *const me, void *arg)
{
    xf_co_t *co;
    xf_co_begin(me);
    printf("co%d begin\n", (int)(intptr_t)me->user_data);
    while (1) {
        XF_LOGI(TAG, "co%d curr: %8" PRIu32, (int)(intptr_t)me->user_data, xf_tick_get_count());
        xf_co_delay_ms(me, 500U);
    }
    printf("co%d end\n", (int)(intptr_t)me->user_data);
    xf_co_end(me);
}

#elif EXAMPLE == EXAMPLE_CO_WAIT_EVENT

xf_stimer_t *stimer_dispatch;

void xf_ps_dispatch_stimer_cb(xf_stimer_t *const s)
{
    xf_ps_dispatch();
    xf_stimer_set_period(stimer_dispatch, XF_STIMER_INFINITY);
}

xf_err_t xf_ps_publish_stimer(xf_event_t *const e)
{
    if (stimer_dispatch) {
        xf_stimer_reset(stimer_dispatch);
        xf_stimer_set_period(stimer_dispatch, 0);
        xf_stimer_set_ready(stimer_dispatch);
    }
    return xf_ps_publish(e);
}

void test_main(void)
{
    xf_tick_t delay_tick;
    UNUSED(delay_tick);
    xf_co_top_init();
    xf_event_gc_init();
    xf_ps_init();
    stimer_dispatch = xf_stimer_create(
                          XF_STIMER_INFINITY,
                          (xf_stimer_cb_t)xf_ps_dispatch_stimer_cb, NULL);
    while (1) {
        xf_co_top_run(NULL);
        /* FIXME 可能存在延时问题 */
        delay_tick = xf_stimer_handler();
        osDelayMs(delay_tick);
        (void)xf_tick_inc(delay_tick);
    }
}

xf_co_state_t co_func_publish(xf_co_t *const me, void *arg);
xf_co_state_t co_func_subscribe(xf_co_t *const me, void *arg);

xf_co_state_t xf_co_main(xf_co_t *const me, void *arg)
{
    xf_co_state_t co_state;
    xf_co_t *co0;
    xf_co_t *co1;
    xf_co_t *co2;
    xf_co_t *co3;
    UNUSED(co_state);
    xf_co_begin(me);
    co0 = xf_co_create(co_func_publish,   0);
    co1 = xf_co_create(co_func_subscribe, 1);
    co2 = xf_co_create(co_func_subscribe, 2);
    co3 = xf_co_create(co_func_subscribe, 3);
    xf_co_resume(me, co0, NULL, co_state);
    xf_co_resume(me, co1, NULL, co_state);
    xf_co_resume(me, co2, NULL, co_state);
    xf_co_resume(me, co3, NULL, co_state);
    xf_co_end(me);
}

#define EVENT_ID_1  1

static xf_event_t s_e_cont_1 = {0};

xf_co_state_t co_func_publish(xf_co_t *const me, void *arg)
{
    xf_co_begin(me);
    printf("co%d begin\n", (int)(intptr_t)me->user_data);
    s_e_cont_1.id = EVENT_ID_1;
    xf_event_set_pool_id(&s_e_cont_1, XF_EVENT_POOL_ID_STATIC);
    while (1) {
        xf_co_delay_ms(me, 1000U);
        XF_LOGI(TAG, "co%d publish:         %8" PRIu32, (int)(intptr_t)me->user_data, EVENT_ID_1);
        xf_ps_publish_stimer(&s_e_cont_1);
    }
    printf("co%d end\n", (int)(intptr_t)me->user_data);
    xf_co_end(me);
}

xf_co_state_t co_func_subscribe(xf_co_t *const me, void *arg)
{
    xf_err_t xf_ret;
    xf_event_t *e;
    xf_co_begin(me);
    printf("co%d begin\n", (int)(intptr_t)me->user_data);
    while (1) {
        xf_co_wait_until_ms(me, EVENT_ID_1, 333U, e, xf_ret);
        if (xf_ret != XF_OK) {
            XF_LOGI(TAG, "co%d wait failed", (int)(intptr_t)me->user_data);
        } else {
            XF_LOGI(TAG, "co%d wait success:    %8" PRIu32, (int)(intptr_t)me->user_data, e->id);
        }
    }
    printf("co%d end\n", (int)(intptr_t)me->user_data);
    xf_co_end(me);
}

#elif EXAMPLE == EXAMPLE_STIMER

void xf_stimer_hi(xf_stimer_t *const me)
{
    XF_LOGI(TAG, "hi: %u", xf_tick_get_count());
    XF_LOGI(TAG, "idle_percentage: %u", xf_stimer_get_idle_percentage());
}

void test_main(void)
{
    xf_tick_t delay_tick;
    xf_stimer_t *stimer;
    stimer = xf_stimer_create(100U, (xf_stimer_cb_t)xf_stimer_hi, NULL);
    xf_stimer_set_repeat_count(stimer, 10U);
    while (1) {
        delay_tick = xf_stimer_handler();
        osDelayMs(delay_tick);
        (void)xf_tick_inc(delay_tick);
    }
}

#elif EXAMPLE == EXAMPLE_PS

void subscr_cb(xf_ps_subscr_t *const s, xf_event_t *e)
{
    XF_LOGI(TAG, "s:            %p", s);
    XF_LOGI(TAG, "s->user_data: %u", (unsigned int)s->user_data);
    XF_LOGI(TAG, "e->id:        %u", (unsigned int)e->id);
    XF_LOGI(TAG, "e->ref_cnt:   %u", (unsigned int)e->ref_cnt);
}

#define EVENT_ID_1  1
#define EVENT_ID_2  2
#define EVENT_ID_3  3
xf_event_t e_cont_1 = {0};
xf_event_t e_cont_2 = {0};
xf_event_t e_cont_3 = {0};

void test_main(void)
{
    xf_ps_subscr_t *s1;
    xf_ps_subscr_t *s2;
    xf_event_init(&e_cont_1, EVENT_ID_1, XF_EVENT_POOL_ID_STATIC);
    xf_event_init(&e_cont_2, EVENT_ID_2, XF_EVENT_POOL_ID_STATIC);
    xf_event_init(&e_cont_3, EVENT_ID_3, XF_EVENT_POOL_ID_STATIC);
    xf_event_gc_init();
    xf_ps_init();
    s1 = xf_ps_create_subscriber(subscr_cb, NULL);
    s2 = xf_ps_create_subscriber(subscr_cb, NULL);
    s1->user_data = (uintptr_t)1;
    s2->user_data = (uintptr_t)2;
    xf_log_printf("\r\n");
    xf_ps_subscribe(s1, EVENT_ID_1);
    xf_ps_subscribe(s1, EVENT_ID_2);
    xf_ps_subscribe(s2, EVENT_ID_2);
    xf_ps_subscribe(s2, EVENT_ID_3);
    while (1) {
        xf_ps_publish(&e_cont_1);
        xf_ps_publish(&e_cont_2);
        xf_ps_publish(&e_cont_3);
        xf_ps_dispatch();
        xf_log_printf("\r\n");
        osDelayMs(1000);
    }
}

#elif EXAMPLE == EXAMPLE_PS_WITH_DATA

#define EVENT_ID_WITH_DATA      1
#define EVENT_ID_WITH_STRING    2

typedef struct {
    xf_event_t  base;
    int         data;
} event_with_data_t;

typedef struct {
    xf_event_t  base;
    char        string[32];
} event_with_string_t;

event_with_data_t s_e_with_data = {0};
event_with_string_t s_e_with_string = {0};

void subscr_cb(xf_ps_subscr_t *const s, xf_event_t *e)
{
    XF_LOGI(TAG, "e->id:                    %u", (unsigned int)e->id);
    if (e->id == EVENT_ID_WITH_DATA) {
        event_with_data_t *e_with_data = (event_with_data_t *)e;
        XF_LOGI(TAG, "e_with_data->data:        %d", e_with_data->data);
    } else {
        event_with_string_t *e_with_string = (event_with_string_t *)e;
        XF_LOGI(TAG, "e_with_string->string:    %s", e_with_string->string);
    }
}

void test_main(void)
{
    xf_ps_subscr_t *s1;
    xf_event_gc_init();
    xf_ps_init();
    xf_event_init(&s_e_with_data.base, EVENT_ID_WITH_DATA, XF_EVENT_POOL_ID_STATIC);
    xf_event_init(&s_e_with_string.base, EVENT_ID_WITH_STRING, XF_EVENT_POOL_ID_STATIC);
    s1 = xf_ps_create_subscriber(subscr_cb, NULL);
    xf_log_printf("\r\n");
    xf_ps_subscribe(s1, EVENT_ID_WITH_DATA);
    xf_ps_subscribe(s1, EVENT_ID_WITH_STRING);
    while (1) {
        s_e_with_data.data = ex_random();
        snprintf(s_e_with_string.string, sizeof(s_e_with_string.string), "\"random: %d\"", s_e_with_data.data);
        xf_ps_publish(&s_e_with_data.base);
        xf_ps_publish(&s_e_with_string.base);
        xf_ps_dispatch();
        xf_log_printf("\r\n");
        osDelayMs(1000);
    }
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
