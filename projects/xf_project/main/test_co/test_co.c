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

#define EXAMPLE_STIMER                  1
#define EXAMPLE_PS                      2
#define EXAMPLE_CO_BASIC                3
#define EXAMPLE_CO_DELAY                4

#define EXAMPLE                         EXAMPLE_CO_DELAY

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

void subscr_cb(xf_ps_subscr_t *const me, xf_event_t *e)
{
    XF_LOGI(TAG, "me:           %p", me);
    XF_LOGI(TAG, "me->user_data:%u", (unsigned int)me->user_data);
    XF_LOGI(TAG, "e->id:        %u", (unsigned int)e->id);
    XF_LOGI(TAG, "e->ref_cnt:   %u", (unsigned int)e->ref_cnt);
}

void test_main(void)
{
#define EVENT_ID_1  1
#define EVENT_ID_2  2
#define EVENT_ID_3  3
    xf_event_t e_cont_1 = {0};
    xf_event_t e_cont_2 = {0};
    xf_event_t e_cont_3 = {0};
    xf_ps_subscr_t *s1;
    xf_ps_subscr_t *s2;
    e_cont_1.id = EVENT_ID_1;
    e_cont_2.id = EVENT_ID_2;
    e_cont_3.id = EVENT_ID_3;
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
