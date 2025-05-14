/**
 * @file xf_ringbuf.h
 * @author catcatBlue (catcatblue@qq.com)
 * @brief 
 * @version 1.0
 * @date 2025-05-14
 * 
 * SPDX-FileCopyrightText: 2025 CompanyNameMagicTag
 * SPDX-License-Identifier: Apache-2.0
 * 
 */

#ifndef __XF_RINGBUF_H__
#define __XF_RINGBUF_H__

/* ==================== [Includes] ========================================== */

#include "xf_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== [Defines] =========================================== */

/* ==================== [Typedefs] ========================================== */

/**
 * @brief 环形缓冲区大小及索引（偏移量）类型定义.
 */
typedef uintptr_t xf_rb_size_t;

/**
 * @brief 环形缓冲区结构体.
 */
typedef struct _xf_ringbuf_t {
    volatile xf_rb_size_t   read_index   : sizeof(xf_rb_size_t) * 8 - 1; /*!< 读操作索引 */
    volatile xf_rb_size_t   read_mirror  : 1;   /*!< 读索引镜像标志 */
    volatile xf_rb_size_t   write_index  : sizeof(xf_rb_size_t) * 8 - 1;  /*!< 写操作索引 */
    volatile xf_rb_size_t   write_mirror : 1;   /*!< 写索引镜像标志 */
    xf_rb_size_t            buf_size;           /*!< buf 大小, 单位字节 */
#if CONFIG_XF_RINGBUF_ZERO_LENGTH_ARRAYS_ENABLE
    char                    p_buf[0];           /*!< 尾随的缓冲区 */
#else
    char                   *p_buf;              /*!< 缓冲区指针 */
#endif
} xf_ringbuf_t;

/* ==================== [Global Prototypes] ================================= */

/**
 * @brief 检查环形缓冲区句柄是否有效.
 *
 * @param p_rb                  环形缓冲区句柄.
 * @return true                 有效
 * @return false                无效
 */
__STATIC_INLINE bool xf_ringbuf_is_valid(const xf_ringbuf_t *p_rb)
{
    return (p_rb) && (p_rb->p_buf) && (p_rb->buf_size > 0);
}

/**
 * @brief 初始化环形缓冲区.
 *
 * @param p_rb                  环形缓冲区句柄.
 * @param p_buf                 缓冲区指针.
 * @param buf_size              缓冲区大小, 单位字节.
 * @return xf_err_t
 *      - XF_OK                 成功
 *      - XF_RET_INVALID_ARG    无效参数
 */
xf_err_t xf_ringbuf_init(
    xf_ringbuf_t *p_rb, void *p_buf, xf_rb_size_t buf_size);

/**
 * @brief 重置环形缓冲区.
 *
 * @param p_rb                  环形缓冲区句柄.
 * @return xf_err_t
 *      - XF_OK                 成功
 *      - XF_RET_INVALID_ARG    无效参数
 */
xf_err_t xf_ringbuf_reset(xf_ringbuf_t *p_rb);

/**
 * @brief 判断环形缓冲区有效数据是否为空.
 *
 * @param p_rb                  环形缓冲区句柄.
 * @return bool
 *      true                    空或无效句柄
 *      false                   非空
 */
bool xf_ringbuf_is_empty(const xf_ringbuf_t *p_rb);

/**
 * @brief 判断环形缓冲区是否已满.
 *
 * @param p_rb                  环形缓冲区句柄.
 * @return bool
 *      true                    满或无效句柄
 *      false                   未满
 */
bool xf_ringbuf_is_full(const xf_ringbuf_t *p_rb);

/**
 * @brief 获取环形缓冲区有效数据字节数.
 *
 * @param p_rb                  环形缓冲区句柄.
 * @return xf_rb_size_t         环形缓冲区有效数据字节数.
 */
xf_rb_size_t xf_ringbuf_get_count(const xf_ringbuf_t *p_rb);

/**
 * @brief 获取环形缓冲区剩余空间字节数.
 *
 * @param p_rb                  环形缓冲区句柄.
 * @return xf_rb_size_t         环形缓冲区剩余空间字节数.
 */
xf_rb_size_t xf_ringbuf_get_free(const xf_ringbuf_t *p_rb);

/**
 * @brief 获取环形缓冲区字节总容量.
 *
 * @param p_rb                  环形缓冲区句柄.
 * @return xf_rb_size_t         环形缓冲区环形缓冲区字节容量.
 */
xf_rb_size_t xf_ringbuf_get_size(const xf_ringbuf_t *p_rb);

/**
 * @brief 向环形缓冲区写入数据.
 *
 * @param p_rb                  环形缓冲区句柄.
 * @param src                   要写入的数据.
 * @param size                  要写入的数据大小(单位：字节).
 * 如果要写入的数据的字节长度大于缓冲区空余大小, 则只会写入 src 内的
 * 索引 0 到 ringbuf_free_size - 1 的数据.
 * @return xf_rb_size_t         实际写入的数据大小(单位：字节).
 */
xf_rb_size_t xf_ringbuf_write(
    xf_ringbuf_t *p_rb, const void *src, xf_rb_size_t size);

/**
 * @brief 强制向环形缓冲区写入数据.
 *
 * @param p_rb                  环形缓冲区句柄.
 * @param src                   要写入的数据.
 * @param size                  要写入的数据大小(单位：字节).
 * 如果要写入的数据的字节长度大于缓冲区总大小, 则只会写入 src 内的
 * 索引 src_size - ringbuf_size 到 src_size - 1 的数据.
 * @return xf_rb_size_t         实际写入的数据大小(单位：字节).
 */
xf_rb_size_t xf_ringbuf_write_force(
    xf_ringbuf_t *p_rb, const void *src, xf_rb_size_t size);

/**
 * @brief 从环形缓冲区读取数据, 同时删除已读取过的数据.
 *
 * @param p_rb                  环形缓冲区句柄.
 * @param[out] dest             读取到的数据的缓冲区.
 * @param size                  要读取的数据大小(单位：字节).
 * @return xf_rb_size_t         实际读取的数据大小(单位：字节).
 */
xf_rb_size_t xf_ringbuf_read(
    xf_ringbuf_t *p_rb, void *dest, xf_rb_size_t size);

/**
 * @brief 从环形缓冲区读取数据, 同时不删除数据.
 *
 * @param p_rb                  环形缓冲区句柄.
 * @param[out] dest             读取到的数据的缓冲区.
 * @param size                  要读取的数据大小(单位：字节).
 * @return xf_rb_size_t         实际读取的数据大小(单位：字节).
 *
 * @note peek 不会改变环形缓冲区索引值. 如果环形缓冲区参数没有改变,
 *      重复 peek 的结果不会改变.
 */
xf_rb_size_t xf_ringbuf_peek(
    xf_ringbuf_t *p_rb, void *dest, xf_rb_size_t size);

/**
 * @brief 从环形缓冲区有效数据的指定索引读取数据, 同时不删除数据.
 *
 * @param p_rb                  环形缓冲区句柄.
 * @param[out] dest             读取到的数据的缓冲区.
 * @param size                  要读取的数据大小(单位：字节).
 * @return xf_rb_size_t         实际读取的数据大小(单位：字节).
 *
 * @note peek 不会改变环形缓冲区索引值. 如果环形缓冲区参数没有改变,
 *      重复 peek 的结果不会改变.
 */
xf_rb_size_t xf_ringbuf_peek_from(
    xf_ringbuf_t *p_rb, xf_rb_size_t offset, void *dest, xf_rb_size_t size);

/**
 * @brief 移除环形缓冲区前 size 字节有效数据.
 *
 * @param p_rb                  环形缓冲区句柄.
 * @param size                  要移除的数据大小(单位：字节).
 * @return xf_rb_size_t         移除的数据大小(单位：字节).
 */
xf_rb_size_t xf_ringbuf_remove(
    xf_ringbuf_t *p_rb, xf_rb_size_t size);

/* ==================== [Macros] ============================================ */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // __XF_RINGBUF_H__
