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

#include "xf_osal.h"

#include <unistd.h>
#include <stdio.h>

#include "test_co.h"
#include "xf_co.h"

/* ==================== [Defines] =========================================== */

static const char *const TAG = "test_co";

#define EXAMPLE_DELAY       3
#define EXAMPLE_AWAIT       4
#define EXAMPLE_MUTEX       5
#define EXAMPLE_SEMAPHORE   6
#define EXAMPLE_SCHEDULER   7

#define EXAMPLE             EXAMPLE_MUTEX

/* ==================== [Typedefs] ========================================== */

/* ==================== [Static Prototypes] ================================= */

/* ==================== [Static Variables] ================================== */

/* ==================== [Macros] ============================================ */

/* ==================== [Global Functions] ================================== */

#if EXAMPLE == EXAMPLE_BASIC

#elif EXAMPLE == EXAMPLE_DELAY

typedef struct test_co {
    xf_co_t         base;       /*!< 必须第一个，名字必须为 base */
    int32_t         count;       /*!< 用户上下文 */
} test_co_t;

test_co_t co1 = {0};

xf_co_async_t co1_func(test_co_t *const me, void *e)
{
    xf_co_begin(me);
    printf("co1 begin\n");
    while (1) {
        me->count++;
        XF_LOGI(TAG, "curr time1: %lu", xf_co_get_tick());
        xf_co_delay(me, 1000);
        XF_LOGI(TAG, "curr time2: %lu", xf_co_get_tick());
        xf_co_delay_ms(me, 1000);
        XF_LOGI(TAG, "curr time3: %lu", xf_co_get_tick());
        xf_co_delay_ms(me, 1000);
        XF_LOGI(TAG, "curr time4: %lu", xf_co_get_tick());
        xf_co_delay_until(me, xf_co_get_tick() + 1000);
        XF_LOGI(TAG, "curr time5: %lu", xf_co_get_tick());
        if (me->count == 2) {
            break;
        }
    }
    me->count = 0;
    printf("co1 end\n");
    xf_co_end(me);
}

void test_main(void)
{
    xf_co_init(&co1, co1_func);

    /* TODO 调度器 */
    while (1) {
        co1_func(&co1, NULL);
    }
}

#elif EXAMPLE == EXAMPLE_AWAIT

typedef struct test_co {
    xf_co_t         base;       /*!< 必须第一个，名字必须为 base */
    int32_t         count;      /*!< 用户上下文 */
} test_co_t;

test_co_t co_D1 = {0};
test_co_t co_D2 = {0};
test_co_t co_D3 = {0};

xf_co_async_t co_func_D3(test_co_t *const me, void *e)
{
    xf_err_t xf_ret;
    xf_co_begin(me);
    printf("co3 begin\n");
    while (1) {
        me->count++;
        xf_co_yield(me);
        XF_LOGI(TAG, "uc3 count: %d", me->count);
        xf_co_yield(me);
        xf_co_delay_ms(me, 1000);
        if (me->count == 2) {
            break;
        }
    }
    me->count = 0;
    printf("co3 end\n");
    xf_co_end(me);
}

xf_co_async_t co_func_D2(test_co_t *const me, void *e)
{
    xf_err_t xf_ret;
    xf_co_begin(me);
    printf("co2 begin\n");
    while (1) {
        XF_LOGI(TAG, "co2 count: %d", me->count);
        xf_co_await(me, co_func_D3, &co_D3, NULL);
        me->count++;
        if (me->count == 2) {
            break;
        }
    }
    me->count = 0;
    printf("co2 end\n");
    xf_co_end(me);
}

xf_co_async_t co_func_D1(test_co_t *const me, void *e)
{
    xf_co_begin(me);
    printf("co1 begin\n");
    while (1) {
        XF_LOGI(TAG, "co1 count: %d", me->count);
        xf_co_await(me, co_func_D2, &co_D2, NULL);
        me->count++;
        if (me->count == 2) {
            break;
        }
    }
    me->count = 0;
    printf("co1 end\n");
    xf_co_end(me);
}

void test_main(void)
{
    xf_co_init(&co_D1, co_func_D1);

    xf_co_state_t co_state = 0;

    /* TODO 调度器 */
    while (1) {
        co_state = co_func_D1(&co_D1, NULL);
        if (co_state == XF_CO_TERMINATED) {
            break;
        }
        usleep(1000);
    }

    printf("done\n");
    while (1) {
        usleep(1000);
    }
}

#elif EXAMPLE == EXAMPLE_MUTEX

typedef struct test_co {
    xf_co_t         base;       /*!< 必须第一个，名字必须为 base */
    int32_t         count;      /*!< 用户上下文 */
} test_co_t;

test_co_t co1 = {0};
test_co_t co2 = {0};

xf_co_mutex_t mutex;
xf_co_mutex_t *p_mutex = &mutex;

xf_co_async_t co_func2(test_co_t *const me, void *e)
{
    xf_err_t xf_ret;
    xf_co_begin(me);
    printf("co2 begin\n");
    while (1) {
        me->count++;
        XF_LOGI(TAG, "co2 count: %d", me->count);
        xf_co_mutex_acquire(me, p_mutex, xf_co_ms_to_tick(123), xf_ret);
        if (xf_ret != XF_OK) {
            XF_LOGI(TAG, "co2 xf_co_mutex_acquire failed");
        } else {
            XF_LOGI(TAG, "co2 xf_co_mutex_acquire success");
            xf_co_delay_ms(me, 123);
            xf_co_mutex_release(me, p_mutex, xf_ret);
            XF_LOGI(TAG, "co2 xf_co_mutex_release");
            xf_co_delay_ms(me, 123 * 2);
        }
        if (me->count == 10) {
            break;
        }
    }
    me->count = 0;
    printf("co2 end\n");
    xf_co_end(me);
}

xf_co_async_t co_func1(test_co_t *const me, void *e)
{
    xf_err_t xf_ret;
    xf_co_begin(me);
    printf("co1 begin\n");
    while (1) {
        me->count++;
        XF_LOGI(TAG, "co1 count: %d", me->count);
        xf_co_mutex_acquire(me, p_mutex, xf_co_ms_to_tick(123), xf_ret);
        if (xf_ret != XF_OK) {
            XF_LOGI(TAG, "co1 xf_co_mutex_acquire failed");
        } else {
            XF_LOGI(TAG, "co1 xf_co_mutex_acquire success");
            xf_co_delay_ms(me, 123 * 2);
            xf_co_mutex_release(me, p_mutex, xf_ret);
            XF_LOGI(TAG, "co1 xf_co_mutex_release");
            xf_co_delay_ms(me, 123);
        }
        if (me->count == 10) {
            break;
        }
    }
    me->count = 0;
    printf("co1 end\n");
    xf_co_end(me);
}

void test_main(void)
{
    xf_co_init(&co1, co_func1);
    xf_co_init(&co2, co_func2);

    xf_co_state_t co_state1 = XF_CO_WAITING;
    xf_co_state_t co_state2 = XF_CO_WAITING;
    xf_co_mutex_init(p_mutex);

    /* TODO 调度器 */
    while (1) {
        if (co_state1 != XF_CO_TERMINATED) {
            co_state1 = co_func1(&co1, NULL);
        }
        if (co_state2 != XF_CO_TERMINATED) {
            co_state2 = co_func2(&co2, NULL);
        }
        if ((co_state1 | co_state2) == XF_CO_TERMINATED) {
            break;
        }
        usleep(1000);
    }

    xf_co_mutex_destroy(p_mutex);

    printf("done\n");
    while (1) {
        usleep(1000);
    }
}

#elif EXAMPLE == EXAMPLE_SEMAPHORE

typedef struct test_co {
    xf_co_t         base;       /*!< 必须第一个，名字必须为 base */
    int32_t         count;      /*!< 用户上下文 */
} test_co_t;

test_co_t co1 = {0};
test_co_t co2 = {0};

xf_co_semaphore_t sem;
xf_co_semaphore_t *p_sem = &sem;

xf_co_async_t co_func2(test_co_t *const me, void *e)
{
    static const char *const TAG = "co2";
    xf_err_t xf_ret;
    xf_co_begin(me);
    XF_LOGI(TAG, "begin\n");
    while (1) {
        me->count++;
        xf_co_delay_ms(me, 180);
        XF_LOGI(TAG, "sem count: %d", xf_co_semaphore_get_count(p_sem));
        xf_co_semaphore_release(me, p_sem, xf_ret);
        XF_LOGI(TAG, "sem count: %d", xf_co_semaphore_get_count(p_sem));
        if (xf_ret != XF_OK) {
            XF_LOGI(TAG, "xf_co_semaphore_release failed");
        } else {
            XF_LOGI(TAG, "xf_co_semaphore_release success");
        }
        if (me->count == 20) {
            break;
        }
    }
    me->count = 0;
    XF_LOGI(TAG, "end\n");
    xf_co_end(me);
}

xf_co_async_t co_func1(test_co_t *const me, void *e)
{
    static const char *const TAG = "co1";
    xf_err_t xf_ret;
    xf_co_begin(me);
    XF_LOGI(TAG, "begin\n");
    while (1) {
        me->count++;
        XF_LOGI(TAG, "sem count: %d", xf_co_semaphore_get_count(p_sem));
        xf_co_semaphore_acquire(me, p_sem, xf_co_ms_to_tick(30), xf_ret);
        XF_LOGI(TAG, "sem count: %d", xf_co_semaphore_get_count(p_sem));
        if (xf_ret != XF_OK) {
            XF_LOGI(TAG, "xf_co_semaphore_acquire failed");
        } else {
            XF_LOGI(TAG, "xf_co_semaphore_acquire success");
            xf_co_delay_ms(me, 30);
        }
        if (me->count == 10) {
            break;
        }
    }
    me->count = 0;
    XF_LOGI(TAG, "end\n");
    xf_co_end(me);
}

void test_main(void)
{
    xf_co_init(&co1, co_func1);
    xf_co_init(&co2, co_func2);

    xf_co_state_t co_state1 = XF_CO_WAITING;
    xf_co_state_t co_state2 = XF_CO_WAITING;
    xf_co_semaphore_init(p_sem, /* max */ 10, /* initial */ 5);

    /* TODO 调度器 */
    while (1) {
        if (co_state1 != XF_CO_TERMINATED) {
            co_state1 = co_func1(&co1, NULL);
        }
        if (co_state2 != XF_CO_TERMINATED) {
            co_state2 = co_func2(&co2, NULL);
        }
        if ((co_state1 | co_state2) == XF_CO_TERMINATED) {
            break;
        }
        usleep(1000);
    }

    printf("done\n");
    XF_LOGI(TAG, "sem count: %d", xf_co_semaphore_get_count(p_sem));
    xf_co_semaphore_destroy(p_sem);

    while (1) {
        usleep(1000);
    }
}

#elif EXAMPLE == EXAMPLE_SCHEDULER

typedef struct test_co {
    xf_co_t         base;       /*!< 必须第一个，名字必须为 base */
    int32_t         count;      /*!< 用户上下文 */
} test_co_t;

xf_co_async_t co_func(test_co_t *const me, void *e)
{
    static const char *const TAG = "co2";
    xf_err_t xf_ret;
    xf_co_begin(me);
    XF_LOGI(TAG, "begin\n");
    while (1) {
        me->count++;
        xf_co_delay_ms(me, 1000);
        XF_LOGI(TAG, "hi");
    }
    me->count = 0;
    XF_LOGI(TAG, "end\n");
    xf_co_end(me);
}

void test_main(void)
{
    test_co_t co_arr[20];
    xf_co_attr_t co_attr = {
        .type = XF_CO_TYPE_UNKNOWN,
        .priority = XF_CO_PRIORITY_LOW,
    };
    int priority_inc = 0;
    for (int i = 0; i < ARRAY_SIZE(co_arr); i++) {
        if ((i + 1) % 3 == 0) {
            priority_inc++;
        }
        co_attr.priority = (xf_co_priority_t)(XF_CO_PRIORITY_LOW + priority_inc);
        xf_co_ctor(xf_co_cast(&co_arr[i]), xf_co_func_cast(co_func), &co_attr);
    }

    extern xf_co_sched_t *sp_sched;

    xf_err_t xf_ret = XF_OK;
    xf_co_t *pos;
    xf_co_t *n;
    xf_co_t *next_co;

    xf_co_sched_co_add(xf_co_cast(sp_sched));
    for (int i = 0; i < ARRAY_SIZE(co_arr); i++) {
        xf_co_sched_co_add(xf_co_cast(&co_arr[i]));
    }
    XF_LOGI(TAG, "---------");
    xf_list_for_each_entry_safe(pos, n, &sp_sched->head, xf_co_t, node_w) {
        XF_LOGI(TAG, "co priority: %d", xf_co_get_attrs_priority(pos));
    }
    XF_LOGI(TAG, "---------");

    for (int i = 0; i < ARRAY_SIZE(co_arr); i++) {
        xf_co_sched_co_del(xf_co_cast(&co_arr[i]));
        XF_LOGI(TAG, "---------");
        xf_list_for_each_entry_safe(pos, n, &sp_sched->head, xf_co_t, node_w) {
            XF_LOGI(TAG, "co priority: %d", xf_co_get_attrs_priority(pos));
        }
        XF_LOGI(TAG, "---------");
    }

    printf("done\n");

    while (1) {
        usleep(1000);
    }
}

#endif

/* ==================== [Static Functions] ================================== */
