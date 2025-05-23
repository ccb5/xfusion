/**
 * @file xf_deque.c
 * @author catcatBlue (catcatblue@qq.com)
 * @brief
 * @version 1.0
 * @date 2025-05-21
 *
 * SPDX-FileCopyrightText: 2025 CompanyNameMagicTag
 * SPDX-License-Identifier: Apache-2.0
 *
 */

/* ==================== [Includes] ========================================== */

#include "xf_deque.h"

/* ==================== [Defines] =========================================== */

#if 1 /* def CONFIG_XF_RINGBUF_EN_FILL */
#   define _EN_READ_AND_CLR             1
#else
#   define _EN_READ_AND_CLR             0
#endif

#ifdef CONFIG_XF_RINGBUF_FILL_VAL
#   define _FILL_VAL                    CONFIG_XF_RINGBUF_FILL_VAL
#else
#   define _FILL_VAL                    0x00
#endif

/* ==================== [Typedefs] ========================================== */

/* ==================== [Static Prototypes] ================================= */

/* ==================== [Static Variables] ================================== */

/* ==================== [Macros] ============================================ */

/* ==================== [Global Functions] ================================== */

xf_err_t xf_deque_init(xf_dq_t *p_dq, void *p_buf, xf_dq_size_t buf_size_bytes)
{
    if ((!p_dq) || (!p_buf) || (buf_size_bytes == 0)) {
        return XF_FAIL;
    }
    p_dq->p_buf = (volatile uint8_t *)p_buf;
    p_dq->buf_size = buf_size_bytes;
    xf_deque_reset(p_dq);
    return XF_OK;
}

xf_err_t xf_deque_reset(xf_dq_t *p_dq)
{
    if (!p_dq) {
        return XF_FAIL;
    }
    p_dq->head = 0;
    p_dq->tail = 0;
    p_dq->head_mirror = XF_DQ_FALSE;
    p_dq->tail_mirror = XF_DQ_FALSE;
#if _EN_READ_AND_CLR
    XF_DQ_MEMSET(p_dq->p_buf, _FILL_VAL, p_dq->buf_size);
#endif
    return XF_OK;
}

xf_dq_size_t xf_deque_get_filled(const xf_dq_t *p_dq)
{
    if ((!p_dq) || (!p_dq->p_buf) || (p_dq->buf_size == 0)) {
        return 0;
    }
    if (XF_DQ_EMPTY(p_dq)) {
        return 0;
    }
    if (XF_DQ_FULL(p_dq)) {
        return p_dq->buf_size;
    }
    if (p_dq->tail >= p_dq->head) {
        return p_dq->tail - p_dq->head;
    }
    return p_dq->buf_size - p_dq->head + p_dq->tail;
}

xf_dq_size_t xf_deque_get_empty(const xf_dq_t *p_dq)
{
    if (!p_dq) {
        return XF_FAIL;
    }
    return p_dq->buf_size - xf_deque_get_filled(p_dq);
}

xf_dq_size_t xf_deque_front_push(xf_dq_t *p_dq, const void *src, xf_dq_size_t size_bytes)
{
    xf_dq_size_t first_part;
    xf_dq_size_t new_head;
    xf_dq_size_t empty_size;
    if ((!p_dq) || (!src) || (size_bytes == 0)) {
        return 0;
    }
    empty_size = xf_deque_get_empty(p_dq);
    if (size_bytes > empty_size) {
        size_bytes = empty_size;
    }
    if (p_dq->head >= size_bytes) {
        p_dq->head -= size_bytes;
        XF_DQ_MEMCPY((void *)(p_dq->p_buf + p_dq->head), src, size_bytes);
        return size_bytes;
    }
    first_part = size_bytes - p_dq->head;
    new_head = p_dq->buf_size - first_part;
    XF_DQ_MEMCPY((void *)(p_dq->p_buf + new_head), (const uint8_t *)src, first_part);
    XF_DQ_MEMCPY((void *)p_dq->p_buf, (const uint8_t *)src + first_part, p_dq->head);
    p_dq->head_mirror ^= 1;
    p_dq->head = new_head;
    return size_bytes;
}

xf_dq_size_t xf_deque_front_push_force(xf_dq_t *p_dq, const void *src, xf_dq_size_t size_bytes)
{
    xf_dq_size_t empty_size;
    if ((!p_dq) || (!src) || (size_bytes == 0)) {
        return 0;
    }
    if (size_bytes > p_dq->buf_size) {
        return 0;
    }
    if (size_bytes > p_dq->buf_size) {
        src = (const uint8_t *)src + (size_bytes - p_dq->buf_size);
        size_bytes = p_dq->buf_size;
    }
    empty_size = xf_deque_get_empty(p_dq);
    if (size_bytes > empty_size) {
        xf_deque_back_remove(p_dq, size_bytes - empty_size);
    }
    return xf_deque_front_push(p_dq, src, size_bytes);
}

xf_dq_size_t xf_deque_front_pop(xf_dq_t *p_dq, void *dest, xf_dq_size_t size_bytes)
{
    xf_dq_size_t first_part;
    xf_dq_size_t new_head;
    xf_dq_size_t filled_size;
    if ((!p_dq) || (!dest) || (size_bytes == 0)) {
        return 0;
    }
    filled_size = xf_deque_get_filled(p_dq);
    if (size_bytes > filled_size) {
        size_bytes = filled_size;
    }
    if (p_dq->head + size_bytes < p_dq->buf_size) {
        XF_DQ_MEMCPY(dest, (const void *)(p_dq->p_buf + p_dq->head), size_bytes);
#if _EN_READ_AND_CLR
        XF_DQ_MEMSET((p_dq->p_buf + p_dq->head), _FILL_VAL, size_bytes);
#endif
        p_dq->head += size_bytes;
        return size_bytes;
    }
    first_part = p_dq->buf_size - p_dq->head;
    new_head = size_bytes - first_part;
    XF_DQ_MEMCPY(dest, (const void *)(p_dq->p_buf + p_dq->head), first_part);
    XF_DQ_MEMCPY((uint8_t *)dest + first_part, (const void *)p_dq->p_buf, new_head);
#if _EN_READ_AND_CLR
    XF_DQ_MEMSET((p_dq->p_buf + p_dq->head), _FILL_VAL, first_part);
    XF_DQ_MEMSET(p_dq->p_buf, _FILL_VAL, size_bytes - first_part);
#endif
    p_dq->head_mirror ^= 1;
    p_dq->head = new_head;
    return size_bytes;
}

xf_dq_size_t xf_deque_front_peek(const xf_dq_t *p_dq, void *dest, xf_dq_size_t size_bytes)
{
    return xf_deque_front_peek_from(p_dq, 0, dest, size_bytes);
}

xf_dq_size_t xf_deque_front_peek_from(
    const xf_dq_t *p_dq, xf_dq_size_t offset, void *dest, xf_dq_size_t size_bytes)
{
    xf_dq_size_t first_part;
    xf_dq_size_t pos;
    xf_dq_size_t filled_size;
    if ((!p_dq) || (!dest) || (size_bytes == 0)) {
        return 0;
    }
    filled_size = xf_deque_get_filled(p_dq);
    if (offset >= filled_size) {
        return 0;
    }
    if (size_bytes + offset > filled_size) {
        size_bytes = filled_size - offset;
    }
    pos = XF_DQ_WRAP(p_dq->head + offset, p_dq->buf_size);
    if (p_dq->buf_size > size_bytes + pos) {
        XF_DQ_MEMCPY(dest, (const void *)(p_dq->p_buf + pos), size_bytes);
        return size_bytes;
    }
    first_part = p_dq->buf_size - pos;
    XF_DQ_MEMCPY(dest, (const void *)(p_dq->p_buf + pos), first_part);
    XF_DQ_MEMCPY((uint8_t *)dest + first_part, (const void *)p_dq->p_buf, size_bytes - first_part);
    return size_bytes;
}

xf_dq_size_t xf_deque_front_remove(xf_dq_t *p_dq, xf_dq_size_t size_bytes)
{
    xf_dq_size_t first_part;
    xf_dq_size_t new_head;
    xf_dq_size_t filled_size;
    if ((!p_dq) || (size_bytes == 0)) {
        return 0;
    }
    filled_size = xf_deque_get_filled(p_dq);
    if (size_bytes > filled_size) {
        size_bytes = filled_size;
    }
    if (p_dq->head + size_bytes < p_dq->buf_size) {
#if _EN_READ_AND_CLR
        XF_DQ_MEMSET((p_dq->p_buf + p_dq->head), _FILL_VAL, size_bytes);
#endif
        p_dq->head += size_bytes;
        return size_bytes;
    }
    first_part = p_dq->buf_size - p_dq->head;
    new_head = size_bytes - first_part;
#if _EN_READ_AND_CLR
    XF_DQ_MEMSET((p_dq->p_buf + p_dq->head), _FILL_VAL, first_part);
    XF_DQ_MEMSET(p_dq->p_buf, _FILL_VAL, size_bytes - first_part);
#endif
    p_dq->head_mirror ^= 1;
    p_dq->head = new_head;
    return size_bytes;
}

xf_dq_size_t xf_deque_back_push(xf_dq_t *p_dq, const void *src, xf_dq_size_t size_bytes)
{
    xf_dq_size_t first_part;
    xf_dq_size_t new_tail;
    xf_dq_size_t empty_size;
    if ((!p_dq) || (!src) || (size_bytes == 0)) {
        return 0;
    }
    empty_size = xf_deque_get_empty(p_dq);
    if (size_bytes > empty_size) {
        size_bytes = empty_size;
    }
    if (p_dq->tail + size_bytes < p_dq->buf_size) {
        XF_DQ_MEMCPY((void *)(p_dq->p_buf + p_dq->tail), src, size_bytes);
        p_dq->tail += size_bytes;
        return size_bytes;
    }
    first_part = p_dq->buf_size - p_dq->tail;
    new_tail = size_bytes - first_part;
    XF_DQ_MEMCPY((void *)(p_dq->p_buf + p_dq->tail), src, first_part);
    XF_DQ_MEMCPY((void *)p_dq->p_buf, (const void *)((const uint8_t *)src + first_part), new_tail);
    p_dq->tail_mirror ^= 1;
    p_dq->tail = new_tail;
    return size_bytes;
}

xf_dq_size_t xf_deque_back_push_force(xf_dq_t *p_dq, const void *src, xf_dq_size_t size_bytes)
{
    xf_dq_size_t empty_size;
    if ((!p_dq) || (!src) || (size_bytes == 0)) {
        return 0;
    }
    if (size_bytes > p_dq->buf_size) {
        // 截断写入长度为最大缓冲区长度
        src = (const uint8_t *)src + (size_bytes - p_dq->buf_size);
        size_bytes = p_dq->buf_size;
    }
    empty_size = xf_deque_get_empty(p_dq);
    if (size_bytes > empty_size) {
        xf_deque_front_remove(p_dq, size_bytes - empty_size);
    }
    return xf_deque_back_push(p_dq, src, size_bytes);
}

xf_dq_size_t xf_deque_back_pop(xf_dq_t *p_dq, void *dest, xf_dq_size_t size_bytes)
{
    xf_dq_size_t filled_size;
    xf_dq_size_t first_part;
    xf_dq_size_t new_tail;
    if ((!p_dq) || (!dest) || (size_bytes == 0)) {
        return 0;
    }
    filled_size = xf_deque_get_filled(p_dq);
    if (size_bytes > filled_size) {
        size_bytes = filled_size;
    }
    if (p_dq->tail >= size_bytes) {
        p_dq->tail -= size_bytes;
        XF_DQ_MEMCPY(dest, (const void *)(p_dq->p_buf + p_dq->tail), size_bytes);
#if _EN_READ_AND_CLR
        XF_DQ_MEMSET((void *)(p_dq->p_buf + p_dq->tail), _FILL_VAL, size_bytes);
#endif
        return size_bytes;
    }
    first_part = size_bytes - p_dq->tail;
    new_tail = p_dq->buf_size - first_part;
    XF_DQ_MEMCPY(dest, (const void *)(p_dq->p_buf + new_tail), first_part);
    XF_DQ_MEMCPY((uint8_t *)dest + first_part, (const void *)p_dq->p_buf, p_dq->tail);
#if _EN_READ_AND_CLR
    XF_DQ_MEMSET(p_dq->p_buf, _FILL_VAL, p_dq->tail);
    XF_DQ_MEMSET((void *)(p_dq->p_buf + new_tail), _FILL_VAL, first_part);
#endif
    p_dq->tail_mirror ^= 1;
    p_dq->tail = new_tail;
    return size_bytes;
}

xf_dq_size_t xf_deque_back_peek(const xf_dq_t *p_dq, void *dest, xf_dq_size_t size_bytes)
{
    return xf_deque_back_peek_from(p_dq, 0, dest, size_bytes);
}

xf_dq_size_t xf_deque_back_peek_from(
    const xf_dq_t *p_dq, xf_dq_size_t offset, void *dest, xf_dq_size_t size_bytes)
{
    xf_dq_size_t filled_size;
    xf_dq_size_t pos;
    xf_dq_size_t first_part;
    if ((!p_dq) || (!dest) || (size_bytes == 0)) {
        return 0;
    }
    filled_size = xf_deque_get_filled(p_dq);
    if (offset >= filled_size) {
        return 0;
    }
    if (size_bytes + offset > filled_size) {
        size_bytes = filled_size - offset;
    }
    pos = XF_DQ_WRAP(p_dq->tail + p_dq->buf_size - offset - size_bytes, p_dq->buf_size);
    if (p_dq->buf_size > pos + size_bytes) {
        XF_DQ_MEMCPY(dest, (const void *)(p_dq->p_buf + pos), size_bytes);
        return size_bytes;
    }
    first_part = p_dq->buf_size - pos;
    XF_DQ_MEMCPY(dest, (const void *)(p_dq->p_buf + pos), first_part);
    XF_DQ_MEMCPY((uint8_t *)dest + first_part, (const void *)p_dq->p_buf, size_bytes - first_part);
    return size_bytes;
}

xf_dq_size_t xf_deque_back_remove(xf_dq_t *p_dq, xf_dq_size_t size_bytes)
{
    xf_dq_size_t filled_size;
    xf_dq_size_t first_part;
    xf_dq_size_t new_tail;
    if ((!p_dq) || (size_bytes == 0)) {
        return 0;
    }
    filled_size = xf_deque_get_filled(p_dq);
    if (size_bytes > filled_size) {
        size_bytes = filled_size;
    }
    if (p_dq->tail >= size_bytes) {
        p_dq->tail -= size_bytes;
#if _EN_READ_AND_CLR
        XF_DQ_MEMSET((void *)(p_dq->p_buf + p_dq->tail), _FILL_VAL, size_bytes);
#endif
        return size_bytes;
    }
    first_part = size_bytes - p_dq->tail;
    new_tail = p_dq->buf_size - first_part;
#if _EN_READ_AND_CLR
    XF_DQ_MEMSET((void *)(p_dq->p_buf + new_tail), _FILL_VAL, first_part);
    XF_DQ_MEMSET((void *)p_dq->p_buf, _FILL_VAL, p_dq->tail);
#endif
    p_dq->tail_mirror ^= 1;
    p_dq->tail = new_tail;
    return size_bytes;
}

/* ==================== [Static Functions] ================================== */
