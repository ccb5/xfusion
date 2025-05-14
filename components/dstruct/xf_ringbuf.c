/**
 * @file xf_ringbuf.c
 * @author catcatBlue (catcatblue@qq.com)
 * @brief 
 * @version 1.0
 * @date 2025-05-14
 * 
 * SPDX-FileCopyrightText: 2025 CompanyNameMagicTag
 * SPDX-License-Identifier: Apache-2.0
 * 
 */

/* ==================== [Includes] ========================================== */

#include <string.h>

#include "xf_ringbuf.h"

/* ==================== [Defines] =========================================== */

/**
 * @brief 读取后清空被读取的数据.
 */
#ifdef CONFIG_XF_RINGBUF_EN_FILL
#   define _EN_READ_AND_CLR             1
#else
#   define _EN_READ_AND_CLR             0
#endif

/**
 * @brief 填充值.
 */
#ifdef CONFIG_XF_RINGBUF_FILL_VAL
#   define _FILL_VAL                    CONFIG_XF_RINGBUF_FILL_VAL
#else
#   define _FILL_VAL                    0x00
#endif

/**
 * @brief 是否检查参数合法性.
 */
#if 1 /* def CONFIG_XF_RINGBUF_EN_CHECK_PARAMETER */
#   define _EN_CHECK_PARAMETER          1
#else
#   define _EN_CHECK_PARAMETER          0
#endif

/* ==================== [Typedefs] ========================================== */

/* ==================== [Static Prototypes] ================================= */

/* ==================== [Static Variables] ================================== */

/* ==================== [Macros] ============================================ */

/* ==================== [Global Functions] ================================= */

xf_err_t xf_ringbuf_init(xf_ringbuf_t *p_rb, void *p_buf, xf_rb_size_t buf_size)
{
#if _EN_CHECK_PARAMETER
    if ((NULL == p_rb) || (NULL == p_buf) || (0 == buf_size)) {
        return XF_ERR_INVALID_ARG;
    }
#endif
#if CONFIG_XF_RINGBUF_ZERO_LENGTH_ARRAYS_ENABLE
    UNUSED(p_buf);
#endif
    *p_rb = (xf_ringbuf_t) {
        .read_index     = 0,
        .read_mirror    = 0,
        .write_index    = 0,
        .write_mirror   = 0,
        .buf_size       = buf_size,
#if !CONFIG_XF_RINGBUF_ZERO_LENGTH_ARRAYS_ENABLE
        .p_buf          = (char *)p_buf,
#endif
    };
#if _EN_READ_AND_CLR
    memset(p_buf, _FILL_VAL, buf_size);
#endif /* _EN_READ_AND_CLR */
    return XF_OK;
}

xf_err_t xf_ringbuf_reset(xf_ringbuf_t *p_rb)
{
#if _EN_CHECK_PARAMETER
    if (!xf_ringbuf_is_valid(p_rb)) {
        return XF_ERR_INVALID_ARG;
    }
#endif
    p_rb->read_index    = 0;
    p_rb->read_mirror   = 0;
    p_rb->write_index   = 0;
    p_rb->write_mirror  = 0;
#if _EN_READ_AND_CLR
    memset(p_buf, _FILL_VAL, buf_size);
#endif /* _EN_READ_AND_CLR */
    return XF_OK;
}

bool xf_ringbuf_is_empty(const xf_ringbuf_t *p_rb)
{
    return (!xf_ringbuf_get_count(p_rb));
}

bool xf_ringbuf_is_full(const xf_ringbuf_t *p_rb)
{
#if _EN_CHECK_PARAMETER
    if (!xf_ringbuf_is_valid(p_rb)) {
        return true;
    }
#endif
    return (xf_ringbuf_get_count(p_rb) == p_rb->buf_size);
}

xf_rb_size_t xf_ringbuf_get_count(const xf_ringbuf_t *p_rb)
{
#if _EN_CHECK_PARAMETER
    if (!xf_ringbuf_is_valid(p_rb)) {
        return 0;
    }
#endif
    if (p_rb->write_index == p_rb->read_index) {
        return (p_rb->write_mirror == p_rb->read_mirror) ? (0) : (p_rb->buf_size);
    }
    return (p_rb->write_index > p_rb->read_index)
           ? (p_rb->write_index - p_rb->read_index)
           : (p_rb->buf_size - (p_rb->read_index - p_rb->write_index));
}

xf_rb_size_t xf_ringbuf_get_free(const xf_ringbuf_t *p_rb)
{
#if _EN_CHECK_PARAMETER
    if (!xf_ringbuf_is_valid(p_rb)) {
        return 0;
    }
#endif
    return p_rb->buf_size - xf_ringbuf_get_count(p_rb);
}

xf_rb_size_t xf_ringbuf_get_size(const xf_ringbuf_t *p_rb)
{
#if _EN_CHECK_PARAMETER
    if (!xf_ringbuf_is_valid(p_rb)) {
        return 0;
    }
#endif
    return p_rb->buf_size;
}

xf_rb_size_t xf_ringbuf_write(
    xf_ringbuf_t *p_rb, const void *src, xf_rb_size_t size)
{
#if _EN_CHECK_PARAMETER
    if ((!xf_ringbuf_is_valid(p_rb)) || (NULL == src) || (0 == size)) {
        return 0;
    }
#endif
    xf_rb_size_t free_sz = xf_ringbuf_get_free(p_rb);
    if (0 == free_sz) {
        return 0;
    }
    if (size > free_sz) {
        size = free_sz;
    }
    /* 当前索引到末尾的空间足以容纳待写入数据 */
    if (p_rb->buf_size - p_rb->write_index > size) {
        memcpy(p_rb->p_buf + p_rb->write_index, src, size);
        p_rb->write_index += size;
        return size;
    }
    /* 当前索引到末尾的空间不足以容纳待写入数据时分两次写入 */
    xf_rb_size_t len_curr2end = p_rb->buf_size - p_rb->write_index;
    xf_rb_size_t len_start2curr = size - len_curr2end;
    memcpy(p_rb->p_buf + p_rb->write_index, src, len_curr2end);
    memcpy(p_rb->p_buf, (char *)src + len_curr2end, len_start2curr);
    p_rb->write_mirror = ~p_rb->write_mirror;
    p_rb->write_index = len_start2curr;
    return size;
}

xf_rb_size_t xf_ringbuf_write_force(
    xf_ringbuf_t *p_rb, const void *src, xf_rb_size_t size)
{
#if _EN_CHECK_PARAMETER
    if ((!xf_ringbuf_is_valid(p_rb)) || (NULL == src) || (0 == size)) {
        return 0;
    }
#endif
    char *p_src = (char *)src;
    xf_rb_size_t free_sz = xf_ringbuf_get_free(p_rb);
    if (size > p_rb->buf_size) {
        p_src = (char *)src + size - p_rb->buf_size;
        size = p_rb->buf_size;
    }
    /* 当前索引到末尾的空间足以容纳待写入数据 */
    if (p_rb->buf_size - p_rb->write_index > size) {
        memcpy(p_rb->p_buf + p_rb->write_index, p_src, size);
        p_rb->write_index += size;
        if (size > free_sz) {
            p_rb->read_index = p_rb->write_index;
        }
        return size;
    }
    /* 当前索引到末尾的空间不足以容纳待写入数据时分两次写入 */
    xf_rb_size_t len_curr2end = p_rb->buf_size - p_rb->write_index;
    xf_rb_size_t len_start2curr = size - len_curr2end;
    memcpy(p_rb->p_buf + p_rb->write_index, p_src, len_curr2end);
    memcpy(p_rb->p_buf, (char *)p_src + len_curr2end, len_start2curr);
    p_rb->write_mirror = ~p_rb->write_mirror;
    p_rb->write_index = len_start2curr;
    if (size > free_sz) {
        if (p_rb->write_index <= p_rb->read_index) {
            p_rb->read_mirror = ~p_rb->read_mirror;
        }
        p_rb->read_index = p_rb->write_index;
    }
    return size;
}

xf_rb_size_t xf_ringbuf_read(
    xf_ringbuf_t *p_rb, void *dest, xf_rb_size_t size)
{
#if _EN_CHECK_PARAMETER
    if ((!xf_ringbuf_is_valid(p_rb)) || (NULL == dest) || (0 == size)) {
        return 0;
    }
#endif
    xf_rb_size_t count = xf_ringbuf_get_count(p_rb);
    if (0 == count) {
        return 0;
    }
    if (size > count) {
        size = count;
    }
    /* 当前索引到末尾的空间足以读取 */
    if (p_rb->buf_size - p_rb->read_index > size) {
        memcpy(dest, p_rb->p_buf + p_rb->read_index, size);
#if _EN_READ_AND_CLR
        memset(p_rb->p_buf + p_rb->read_index, _FILL_VAL, size);
#endif
        p_rb->read_index += size;
        return size;
    }
    /* 当前索引到末尾的空间不足以读取时分两次读取 */
    xf_rb_size_t len_curr2end = p_rb->buf_size - p_rb->read_index;
    xf_rb_size_t len_start2curr = size - len_curr2end;
    memcpy(dest, p_rb->p_buf + p_rb->read_index, len_curr2end);
    memcpy((char *)dest + len_curr2end, p_rb->p_buf, len_start2curr);
#if _EN_READ_AND_CLR
    memset(p_rb->p_buf, _FILL_VAL, len_start2curr);
    memset(p_rb->p_buf + p_rb->read_index, _FILL_VAL, len_curr2end);
#endif /* _EN_READ_AND_CLR */
    p_rb->read_mirror = ~p_rb->read_mirror;
    p_rb->read_index = len_start2curr;
    return size;
}

xf_rb_size_t xf_ringbuf_peek(
    xf_ringbuf_t *p_rb, void *dest, xf_rb_size_t size)
{
#if _EN_CHECK_PARAMETER
    if ((!xf_ringbuf_is_valid(p_rb)) || (NULL == dest) || (0 == size)) {
        return 0;
    }
#endif
    xf_rb_size_t count = xf_ringbuf_get_count(p_rb);
    if (0 == count) {
        return 0;
    }
    if (size > count) {
        size = count;
    }
    /* 当前索引到末尾的空间足以读取 */
    if (p_rb->buf_size - p_rb->read_index > size) {
        memcpy(dest, p_rb->p_buf + p_rb->read_index, size);
        // p_rb->read_index += size;
        return size;
    }
    /* 当前索引到末尾的空间不足以读取时分两次读取 */
    xf_rb_size_t len_curr2end = p_rb->buf_size - p_rb->read_index; /*!< 当前到 buf 末尾的字节数 */
    xf_rb_size_t len_start2curr = size - len_curr2end; /*!< buf 开头到当前方向需要读的字节数 */
    memcpy(dest, p_rb->p_buf + p_rb->read_index, len_curr2end);
    memcpy((char *)dest + len_curr2end, p_rb->p_buf, len_start2curr);
    // p_rb->read_mirror = ~p_rb->read_mirror;
    // p_rb->read_index = len_start2curr;
    return size;
}

xf_rb_size_t xf_ringbuf_peek_from(
    xf_ringbuf_t *p_rb, xf_rb_size_t offset, void *dest, xf_rb_size_t size)
{
#if _EN_CHECK_PARAMETER
    if ((!xf_ringbuf_is_valid(p_rb)) || (NULL == dest) || (0 == size)) {
        return 0;
    }
#endif
    xf_rb_size_t count = xf_ringbuf_get_count(p_rb);
    if ((0 == count) || (offset >= count)) {
        return 0;
    }
    if (size + offset > count) {
        size = count - offset;
    }
    xf_rb_size_t read_index_temp = p_rb->read_index + offset;
    /* 当前索引到末尾的空间足以读取 */
    if (p_rb->buf_size - read_index_temp > size) {
        memcpy(dest, p_rb->p_buf + read_index_temp, size);
        /* 不改变读索引 */
        return size;
    }
    /* 当前索引到末尾的空间不足以读取时分两次读取 */
    xf_rb_size_t len_curr2end = p_rb->buf_size - read_index_temp;
    xf_rb_size_t len_start2curr = size - len_curr2end;
    memcpy(dest, p_rb->p_buf + read_index_temp, len_curr2end);
    memcpy((char *)dest + len_curr2end, p_rb->p_buf, len_start2curr);
    /* 不改变读镜像与读索引 */
    return size;
}

xf_rb_size_t xf_ringbuf_remove(
    xf_ringbuf_t *p_rb, xf_rb_size_t size)
{
#if _EN_CHECK_PARAMETER
    if ((!xf_ringbuf_is_valid(p_rb)) || (0 == size)) {
        return 0;
    }
#endif
    xf_rb_size_t count = xf_ringbuf_get_count(p_rb);
    if (0 == count) {
        return 0;
    }
    if (size > count) {
        size = count;
    }
    /* 当前索引到末尾的空间足以读取 */
    if (p_rb->buf_size - p_rb->read_index > size) {
#if _EN_READ_AND_CLR
        memset(p_rb->p_buf + p_rb->read_index, _FILL_VAL, size);
#endif
        p_rb->read_index += size;
        return size;
    }
    /* 当前索引到末尾的空间不足以读取时分两次读取 */
    xf_rb_size_t len_curr2end = p_rb->buf_size - p_rb->read_index;
    xf_rb_size_t len_start2curr = size - len_curr2end;
#if _EN_READ_AND_CLR
    memset(p_rb->p_buf, _FILL_VAL, len_start2curr);
    memset(p_rb->p_buf + p_rb->read_index, _FILL_VAL, len_curr2end);
#endif /* _EN_READ_AND_CLR */
    p_rb->read_mirror = ~p_rb->read_mirror;
    p_rb->read_index = len_start2curr;
    return size;
}

/* ==================== [Static Functions] ================================== */
