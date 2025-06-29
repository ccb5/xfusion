/**
 * @file xf_deque.h
 * @author catcatBlue (catcatblue@qq.com)
 * @brief
 * @version 1.0
 * @date 2025-05-21
 *
 * SPDX-FileCopyrightText: 2025 CompanyNameMagicTag
 * SPDX-License-Identifier: Apache-2.0
 *
 */

/*
    ATTENTION:

        暂不允许同时关闭以下两个配置。
        关闭这两个配置主要用于将 xf_dq_t 作为基类使用。

    - XF_DEQUE_ENABLE_ZERO_LENGTH_ARRAYS
    - XF_DEQUE_ENABLE_BUFFER_POINTER

    不允许同时关闭的原因：
    如果外部：
    ```c
    typedef struct xf_dq_trailing_buf  {
        xf_dq_t                 base;
        uintptr_t               buf[16];
    } xf_dq_trailing_buf_t;
    ```
    但内部通过以下结构体：
    ```c
    typedef struct xf_deque_probe_trailing  {
        xf_dq_t                 base;
        uintptr_t               uiptr;
    } xf_dq_probe_t;
    ```
    探测时，可能会遇到结构体对齐问题，
    C89 条件下没有通用解决办法，因此不使用这种方式。
 */

#ifndef __XF_DEQUE_H__
#define __XF_DEQUE_H__

/* ==================== [Includes] ========================================== */

#include "../common/xf_common.h"
#include "../std/xf_std.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== [Defines] =========================================== */

/* 目前禁止开启 XF_DEQUE_ENABLE_ZERO_LENGTH_ARRAYS 和关闭 XF_DEQUE_ENABLE_BUFFER_POINTER */
#define XF_DEQUE_ENABLE_ZERO_LENGTH_ARRAYS  0
#define XF_DEQUE_ENABLE_BUFFER_POINTER      1

/* ==================== [Typedefs] ========================================== */

typedef uint16_t xf_dq_size_t;

/**
 * @brief 双向队列（deque）结构体。
 *
 * 支持从头部（front）和尾部（back）以字节单元进行入队/出队操作。
 */
typedef struct xf_dq {
#if (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L))
    volatile xf_dq_size_t   head        : (sizeof(xf_dq_size_t) * 8) - 1;
    volatile xf_dq_size_t   head_mirror : 1;
    volatile xf_dq_size_t   tail        : (sizeof(xf_dq_size_t) * 8) - 1;
    volatile xf_dq_size_t   tail_mirror : 1;
#else
    volatile xf_dq_size_t   head;
    volatile xf_dq_size_t   tail;
    volatile bool_t         head_mirror;
    volatile bool_t         tail_mirror;
#endif
    xf_dq_size_t            buf_size;       /*!< 缓冲区总大小（字节） */
#if XF_DEQUE_ENABLE_BUFFER_POINTER
    volatile uint8_t       *p_buf;              /*!< 缓冲区指针 */
#elif XF_DEQUE_ENABLE_ZERO_LENGTH_ARRAYS
    uint8_t                 p_buf[0];           /*!< 尾随的缓冲区 */
#endif
} xf_dq_t;

/* ==================== [Global Prototypes] ================================= */

xf_err_t xf_deque_init(xf_dq_t *p_dq, void *p_buf, xf_dq_size_t buf_size_bytes);
xf_err_t xf_deque_reset(xf_dq_t *p_dq);
bool_t xf_deque_is_empty(const xf_dq_t *p_dq);
bool_t xf_deque_is_full(const xf_dq_t *p_dq);
xf_dq_size_t xf_deque_get_filled(const xf_dq_t *p_dq);
xf_dq_size_t xf_deque_get_empty(const xf_dq_t *p_dq);
xf_dq_size_t xf_deque_get_size(const xf_dq_t *p_dq);

/**
 * @note
 * - fifo: back_push + front_pop 在单生产者单消费者情况下支持无锁操作
 * - lifo: back_push + back_pop
 */

xf_dq_size_t xf_deque_front_push(xf_dq_t *p_dq, const void *src, xf_dq_size_t size_bytes);
xf_dq_size_t xf_deque_front_push_force(xf_dq_t *p_dq, const void *src, xf_dq_size_t size_bytes);
xf_dq_size_t xf_deque_front_pop(xf_dq_t *p_dq, void *dest, xf_dq_size_t size_bytes);
xf_dq_size_t xf_deque_front_peek(const xf_dq_t *p_dq, void *dest, xf_dq_size_t size_bytes);
xf_dq_size_t xf_deque_front_peek_from(
    const xf_dq_t *p_dq, xf_dq_size_t offset, void *dest, xf_dq_size_t size_bytes);
xf_dq_size_t xf_deque_front_remove(xf_dq_t *p_dq, xf_dq_size_t size_bytes);

xf_dq_size_t xf_deque_back_push(xf_dq_t *p_dq, const void *src, xf_dq_size_t size_bytes);
xf_dq_size_t xf_deque_back_push_force(xf_dq_t *p_dq, const void *src, xf_dq_size_t size_bytes);
xf_dq_size_t xf_deque_back_pop(xf_dq_t *p_dq, void *dest, xf_dq_size_t size_bytes);
xf_dq_size_t xf_deque_back_peek(const xf_dq_t *p_dq, void *dest, xf_dq_size_t size_bytes);
xf_dq_size_t xf_deque_back_peek_from(
    const xf_dq_t *p_dq, xf_dq_size_t offset, void *dest, xf_dq_size_t size_bytes);
xf_dq_size_t xf_deque_back_remove(xf_dq_t *p_dq, xf_dq_size_t size_bytes);

/* ==================== [Macros] ============================================ */

#define XF_DQ_EMPTY(q)                  (((q)->head == (q)->tail) && ((q)->head_mirror == (q)->tail_mirror))
#define XF_DQ_FULL(q)                   (((q)->head == (q)->tail) && ((q)->head_mirror != (q)->tail_mirror))

#define XF_DQ_MEMCPY(dst, src, size)    xf_memcpy((void *)(dst), (const void *)(src), (size))
#define XF_DQ_MEMSET(dst, c, size)      xf_memset((void *)(dst), (c), (size))

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __XF_DEQUE_H__ */
