/**
 * @file xf_bit.h
 * @author catcatBlue (catcatblue@qq.com)
 * @brief
 * @version 1.0
 * @date 2025-05-14
 *
 * SPDX-FileCopyrightText: 2025 CompanyNameMagicTag
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#ifndef __XF_BIT_H__
#define __XF_BIT_H__

/* ==================== [Includes] ========================================== */

#include "xf_common_internal.h"
#include "xf_types.h"
#include "xf_macro_definition.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== [Defines] =========================================== */

#if (!defined(BIT0))
#define BIT0    0x00000001UL
#define BIT1    0x00000002UL
#define BIT2    0x00000004UL
#define BIT3    0x00000008UL
#define BIT4    0x00000010UL
#define BIT5    0x00000020UL
#define BIT6    0x00000040UL
#define BIT7    0x00000080UL
#define BIT8    0x00000100UL
#define BIT9    0x00000200UL
#define BIT10   0x00000400UL
#define BIT11   0x00000800UL
#define BIT12   0x00001000UL
#define BIT13   0x00002000UL
#define BIT14   0x00004000UL
#define BIT15   0x00008000UL
#define BIT16   0x00010000UL
#define BIT17   0x00020000UL
#define BIT18   0x00040000UL
#define BIT19   0x00080000UL
#define BIT20   0x00100000UL
#define BIT21   0x00200000UL
#define BIT22   0x00400000UL
#define BIT23   0x00800000UL
#define BIT24   0x01000000UL
#define BIT25   0x02000000UL
#define BIT26   0x04000000UL
#define BIT27   0x08000000UL
#define BIT28   0x10000000UL
#define BIT29   0x20000000UL
#define BIT30   0x40000000UL
#define BIT31   0x80000000UL
#endif /* (!defined(BIT0)) */

#if (!defined(BIT32)) && XF_COMMON_ENABLE_64BITS
#define BIT32   (0x00000001ULL << 32)
#define BIT33   (0x00000002ULL << 32)
#define BIT34   (0x00000004ULL << 32)
#define BIT35   (0x00000008ULL << 32)
#define BIT36   (0x00000010ULL << 32)
#define BIT37   (0x00000020ULL << 32)
#define BIT38   (0x00000040ULL << 32)
#define BIT39   (0x00000080ULL << 32)
#define BIT40   (0x00000100ULL << 32)
#define BIT41   (0x00000200ULL << 32)
#define BIT42   (0x00000400ULL << 32)
#define BIT43   (0x00000800ULL << 32)
#define BIT44   (0x00001000ULL << 32)
#define BIT45   (0x00002000ULL << 32)
#define BIT46   (0x00004000ULL << 32)
#define BIT47   (0x00008000ULL << 32)
#define BIT48   (0x00010000ULL << 32)
#define BIT49   (0x00020000ULL << 32)
#define BIT50   (0x00040000ULL << 32)
#define BIT51   (0x00080000ULL << 32)
#define BIT52   (0x00100000ULL << 32)
#define BIT53   (0x00200000ULL << 32)
#define BIT54   (0x00400000ULL << 32)
#define BIT55   (0x00800000ULL << 32)
#define BIT56   (0x01000000ULL << 32)
#define BIT57   (0x02000000ULL << 32)
#define BIT58   (0x04000000ULL << 32)
#define BIT59   (0x08000000ULL << 32)
#define BIT60   (0x10000000ULL << 32)
#define BIT61   (0x20000000ULL << 32)
#define BIT62   (0x40000000ULL << 32)
#define BIT63   (0x80000000ULL << 32)
#endif /* (!defined(BIT32)) */

/* ==================== [Typedefs] ========================================== */

/* ==================== [Global Prototypes] ================================= */

/* ==================== [Macros] ============================================ */

/**
 * @brief 位操作.
 *
 * @details
 * 位操作宏名称类似于:
 *      BIT[S]_<[GET]/[GET_MDF]/[SET]/[FLIP]><[0]/[1]/null>
 * 1. 命名空间
 *    1. BIT_:       表示对**一个**位的操作.
 *    2. BITS_:      表示对**多个**位的操作.
 *    3. BITSn_:     表示对**多个**位中的**n个位**的操作.
 * 2. 操作集
 *    1. 设置
 *          SET、FLIP
 *       1. 会改变源的值, 因此对象必须可读可写, 因此不能是常数.
 *       2. SET0、SET1 表示需要目标值是 0、1. 不带后缀表示目标值由用户输入.
 *       3. FLIP 表示翻转.
 *    2. 获取
 *          GET、GET_MDF、MASK
 *       1. 不会改变原值, 源可以是常数.
 *       2. GET_MDF 表示获取获取修改后的值, 不修改原值.
 *       3. MASK 只用于获取低 n 位为 1 的掩码.
 *    3. BIT 中的 n
 *       1. 表示对 bit_n 的操作, 从 0 开始.
 *    4. BITS 中的 bitmask
 *       1. bitmask 中为 1 的 bit 表示当前操作的操作对象.
 */

#define BIT_IE                          (1UL)  /*!< 或 (uint32_t)1 */

#if defined(__ASSEMBLER__)
#   undef  BIT_IE
#   define BIT_IE 1
#endif

#if !defined(BIT)
#   define BIT(n)                       (BIT_IE << (n))
#endif

#if !defined(BIT_GET)
/**
 * @brief 获取 src 的 bit_n.
 *
 * @param src 被读取的变量.
 * @param n 比特下标, 从 0 开始.
 *
 * @return src 的 bit_n 上的值.
 */
#   define BIT_GET(src, n)              (((src) >> (n)) & BIT0)
#endif

#if !defined(BIT_GET_MDF0)
/**
 * @brief 获取将 src 的 bit_n 置 0 后的值.
 *
 * @param src 被读取的变量.
 * @param n 比特下标, 从 0 开始.
 *
 * @return src 的 bit_n 置 0 后的值.
 */
#   define BIT_GET_MDF0(src, n)         ((src) & ~BIT(n))
#endif

#if !defined(BIT_GET_MDF1)
/**
 * @brief 获取将 src 的 bit_n 置 1 后的值.
 *
 * @param src 被读取的变量.
 * @param n 比特下标, 从 0 开始.
 *
 * @return src 的 bit_n 置 1 后的值.
 */
#   define BIT_GET_MDF1(src, n)         ((src) | BIT(n))
#endif

#if !defined(BIT_GET_MDF)
/**
 * @brief 获取将 src 的 bit_n 置 value 后的值.
 *
 * @param src 被读取的变量.
 * @param n 比特下标, 从 0 开始.
 * @param value 0 或 1.
 *
 * @return src 的 bit_n 置 value 后的值.
 */
#   define BIT_GET_MDF(src, n, value)   (BIT_GET_MDF0(src, n) | (!!(value) << (n)))
#endif

#if !defined(BIT_GET_FLIP)
/**
 * @brief 获取翻转 src 的 bit_n 后的值.
 *
 * @param src 被读取的变量.
 * @param n 比特下标, 从 0 开始.
 *
 * @return 翻转 src 的 bit_n 后的值.
 */
#   define BIT_GET_FLIP(src, n)         ((src) ^ BIT(n))
#endif

#if !defined(BIT_SET0)
/**
 * @brief 设置 src 的 bit_n 为 0.
 *
 * @param src 被读写的变量.
 * @param n 比特下标, 从 0 开始.
 */
#   define BIT_SET0(src, n)             ((src) = BIT_GET_MDF0(src, n))
#endif

#if !defined(BIT_SET1)
/**
 * @brief 设置 src 的 bit_n 为 1.
 *
 * @param src 被读写的变量.
 * @param n 比特下标, 从 0 开始.
 */
#   define BIT_SET1(src, n)             ((src) = BIT_GET_MDF1(src, n))
#endif

#if !defined(BIT_SET)
/**
 * @brief 设置 src 的 bit_n 为 value.
 *
 * @param src 被读写的变量.
 * @param n 比特下标, 从 0 开始.
 * @param value 0 或 1.
 */
#   define BIT_SET(src, n, value)       ((src) = BIT_GET_MDF(src, n, value))
#endif

#if !defined(BIT_FLIP)
/**
 * @brief 翻转 src 的 bit_n.
 *
 * @param src 被翻转的变量.
 * @param n 比特下标, 从 0 开始.
 */
#   define BIT_FLIP(src, n)             ((src) = BIT_GET_FLIP(src, n))
#endif

#if !defined(BIT_GET_LSH)
/**
 * @brief 获取左移 src 1 位的结果.
 *
 * @param src 被左移的变量.
 *
 * @return 左移 src 1 位 后的结果.
 */
#   define BIT_GET_LSH(src)             ((src) << 1)
#endif

#if !defined(BIT_GET_RSH)
/**
 * @brief 获取右移 src 1 位的结果.
 *
 * @param src 被右移的变量.
 *
 * @return 右移 src 1 位 后的结果.
 */
#   define BIT_GET_RSH(src)             ((src) >> 1)
#endif

#if !defined(BIT_MASK)
/**
 * @brief 获取低 n 位为 1 的位掩码.
 *
 * @param n 小于等于 32 的数.
 *
 * @return 低 n 位为 1 的位掩码.
 *
 * @code{markdown}
 * BIT_MASK(6)  == 0x003f ==          0b00111111
 * BIT_MASK(13) == 0x1fff == 0b00011111 11111111
 * @endcode
 */
#   define BIT_MASK(n)                 (((n) < (8 * sizeof(BIT_IE))) ? (BIT(n) - 1) : ~(BIT_IE - 1))
#endif

#if !defined(BIT_MASK_LSH)
/**
 * @brief 获取低 n 位为 1 的位掩码, 并左移 offset 位.
 *
 * @param n 小于等于 32 的数.
 * @param offset 左移位数.
 *
 * @return 高 n 位为 1 的位掩码.
 */
#   define BIT_MASK_LSH(n, offset)      (BIT_MASK(n) << (offset))
#endif

#if !defined(BITS_GET_MDF0)
/**
 * @brief 获取设置 src 的对应位掩码 bitmask 为 1 的地方为 0 后的值.
 *
 * @param src 被设置的变量.
 * @param bitmask 位掩码. 注意! 为 1 的位表示需要被设为 0 的位.
 *
 * @note
 * 1. 位掩码为 1 的位表示需要被设为 0 的位.
 */
#   define BITS_GET_MDF0(src, bitmask)  ((src) & ~(bitmask))
#endif

#if !defined(BITS_GET_MDF1)
/**
 * @brief 获取设置 src 的对应位掩码 bitmask 为 1 的地方为 1 后的值.
 *
 * @param src 被设置的变量.
 * @param bitmask 位掩码. 注意! 为 1 的位表示需要被设为 1 的位.
 *
 * @note
 * 1. 位掩码为 1 的位表示需要被设为 1 的位.
 */
#   define BITS_GET_MDF1(src, bitmask)  ((src) | (bitmask))
#endif

#if !defined(BITS_GET_MDF_FLIP)
/**
 * @brief 获取翻转 src 的 bitmask 为 1 的位后的值.
 *
 * @param src 被设置的变量.
 * @param bitmask 位掩码. 注意! 为 1 的位表示需要被翻转的位.
 *
 * @note
 * 1. 位掩码为 1 的位表示需要被翻转的位.
 */
#   define BITS_GET_MDF_FLIP(src, bitmask) \
                                        ((src) ^ (bitmask))
#endif

#if !defined(BITS_SET0)
/**
 * @brief 设置 src 的对应位掩码 bitmask 为 1 的地方为 0.
 *
 * @param src 被设置的变量.
 * @param bitmask 位掩码. 注意! 为 1 的位表示需要被设为 0 的位.
 *
 * @note
 * 1. 位掩码为 1 的位表示需要被设为 0 的位.
 *
 * @code{markdown}
 * uint32_t src = 0xffff;
 * BITS_SET0(src, 0b0011 0110 0101 1010) == 0b11001001 10100101
 * src = 0xffff;
 * BITS_SET0(src, BIT_MASK(4) << 3)      == 0b11111111 10000111
 * @endcode
 */
#   define BITS_SET0(src, bitmask)      ((src) = BITS_GET_MDF0(src, bitmask))
#endif

#if !defined(BITS_SET1)
/**
 * @brief 设置 src 的对应位掩码 bitmask 为 1 的地方为 1.
 *
 * @param src 被设置的变量.
 * @param bitmask 位掩码. 注意! 为 1 的位表示需要被设为 1 的位.
 *
 * @note
 * 1. 位掩码为 1 的位表示需要被设为 1 的位.
 *
 * @code{markdown}
 * uint32_t src = 0x0000;
 * BITS_SET1(src, 0b0011 0110 0101 1010) == 0b00110110 01011010
 * src = 0x0000;
 * BITS_SET1(src, BIT_MASK(4) << 3)      == 0b00000000 01111000
 * @endcode
 */
#   define BITS_SET1(src, bitmask)      ((src) = BITS_GET_MDF1(src, bitmask))
#endif

#if !defined(BITS_FLIP)
/**
 * @brief 翻转 src 的 bitmask 为 1 的位.
 *
 * @param src 被设置的变量.
 * @param bitmask 位掩码. 注意! 为 1 的位表示需要被翻转的位.
 *
 * @note
 * 1. 位掩码为 1 的位表示需要被翻转的位.
 */
#   define BITS_FLIP(src, bitmask)      ((src) = BITS_GET_MDF_FLIP(src, bitmask))
#endif

#if !defined(BITSn_GET_LSH)
/**
 * @brief 获取 src 保留低 n 位(共有 n 位)并左移 offset 位后的值.
 *
 * left shifts.
 *
 * @param src 被读取的源.
 * @param n 需要读取 n 位.
 * @param offset 偏移量. 从 bit0 起计算.
 *
 * @return src 内, 从 offset 位起, 共 n 位数据.
 */
#   define BITSn_GET_LSH(src, n, offset) \
                                        (((src) & BIT_MASK(n)) << (offset))
#endif

#if !defined(BITSn_GET_RSH)
/**
 * @brief 获取 src 的从 offset 位起共 n 位数据.
 *
 * right shifts.
 *
 * @param src 被读取的源.
 * @param n 需要保留 n 位.
 * @param offset 偏移量. 从 bit0 起计算.
 *
 * @return src 内, 从 offset 位起, 共 n 位数据.
 *
 * @code{markdown}
 * BITS_GET_RSH(0b0011 1010 1111 0010, 7, 5) == 0x57 == 0b01010111
 *
 * bit:                54 3210  : bit == 5
 * src:  0b0011 1010 1111 0010  : src == 0x3af2
 *              ↓      ↓
 * num:         7654 321        : n   == 7
 * ————————————————————————————————————————————
 * ret:     0b0 1010 111        : ret == 0x57
 * @endcode
 */
#   define BITSn_GET_RSH(src, n, offset) \
                                        (((src) >> (offset)) & BIT_MASK(n))
#endif

#if !defined(BITSn_GET_MDF)
/**
 * @brief 获取将 src 的 offset 位起共 n 位修改为 value 后的值.
 *
 * @param src 被读取的源.
 * @param n 需要修改的位数.
 * @param offset 偏移量. 从 bit0 起计算.
 * @param value src 的 offset 位起 n 位源需要修改到的目标值.
 *
 * @return src 的 offset 位起 n 位修改为 value 的值.
 *
 * @code{markdown}
 *  BITSn_GET_MDF(src       , n, offset, value     )
 *  BITSn_GET_MDF(0x4c5ca6d2, 8, 13    , 0b10101111)
 *
 *  src:    0100 1100 0101 1100 1010 0110 1101 0010 == 0x4c5ca6d2
 *                       1 0101 111                 == ((value & BIT_MASK(8)) << 13)
 *                       ↓ ↓↓↓↓ ↓↓↓
 *  ret:    0100 1100 0101 0101 1110 0110 1101 0010 == 0x4c55e6d2
 * @endcode
 *
 * @note
 * 1. n + offset 不要大于 32 位（如果没有设 BIT_IE 为 1ULL）.
 */
#   define BITSn_GET_MDF(src, n, offset, value) \
                                        (BITS_GET_MDF0((src), BIT_MASK_LSH((n), (offset))) \
                                            | BITSn_GET_LSH((value), (n), (offset)))
#endif

#if !defined(BITSn_SET)
/**
 * @brief 设置 src 的 offset 位起共 n 位为 value.
 *
 * @see BITSn_GET_MDF @ref BITSn_GET_MDF
 * @note
 * 1. n + offset 不要大于 32 位（如果没有设 BIT_IE 为 1ULL）.
 */
#   define BITSn_SET(src, n, offset, value) \
                                        ((src) = BITSn_GET_MDF((src), (n), (offset), (value)))
#endif

#if XF_COMMON_ENABLE_64BITS

#define BIT64_IE                        (1ULL)  /*!< 或 (uint64_t)1 */

#if !defined(BIT64)
#   define BIT64(n)                     (BIT64_IE << (n))
#endif

#if !defined(BIT64_GET)
/**
 * @brief 获取 src 的 bit64_n.
 *
 * @param src 被读取的变量.
 * @param n 比特下标, 从 0 开始.
 *
 * @return src 的 bit64_n 上的值.
 */
#   define BIT64_GET(src, n)              (((src) >> (n)) & BIT0)
#endif

#if !defined(BIT64_GET_MDF0)
/**
 * @brief 获取将 src 的 bit64_n 置 0 后的值.
 *
 * @param src 被读取的变量.
 * @param n 比特下标, 从 0 开始.
 *
 * @return src 的 bit64_n 置 0 后的值.
 */
#   define BIT64_GET_MDF0(src, n)         ((src) & ~BIT(n))
#endif

#if !defined(BIT64_GET_MDF1)
/**
 * @brief 获取将 src 的 bit64_n 置 1 后的值.
 *
 * @param src 被读取的变量.
 * @param n 比特下标, 从 0 开始.
 *
 * @return src 的 bit64_n 置 1 后的值.
 */
#   define BIT64_GET_MDF1(src, n)         ((src) | BIT(n))
#endif

#if !defined(BIT64_GET_MDF)
/**
 * @brief 获取将 src 的 bit64_n 置 value 后的值.
 *
 * @param src 被读取的变量.
 * @param n 比特下标, 从 0 开始.
 * @param value 0 或 1.
 *
 * @return src 的 bit64_n 置 value 后的值.
 */
#   define BIT64_GET_MDF(src, n, value)   (BIT64_GET_MDF0(src, n) | (!!(value) << (n)))
#endif

#if !defined(BIT64_GET_FLIP)
/**
 * @brief 获取翻转 src 的 bit64_n 后的值.
 *
 * @param src 被读取的变量.
 * @param n 比特下标, 从 0 开始.
 *
 * @return 翻转 src 的 bit64_n 后的值.
 */
#   define BIT64_GET_FLIP(src, n)         ((src) ^ BIT(n))
#endif

#if !defined(BIT64_SET0)
/**
 * @brief 设置 src 的 bit64_n 为 0.
 *
 * @param src 被读写的变量.
 * @param n 比特下标, 从 0 开始.
 */
#   define BIT64_SET0(src, n)             ((src) = BIT64_GET_MDF0(src, n))
#endif

#if !defined(BIT64_SET1)
/**
 * @brief 设置 src 的 bit64_n 为 1.
 *
 * @param src 被读写的变量.
 * @param n 比特下标, 从 0 开始.
 */
#   define BIT64_SET1(src, n)             ((src) = BIT64_GET_MDF1(src, n))
#endif

#if !defined(BIT64_SET)
/**
 * @brief 设置 src 的 bit64_n 为 value.
 *
 * @param src 被读写的变量.
 * @param n 比特下标, 从 0 开始.
 * @param value 0 或 1.
 */
#   define BIT64_SET(src, n, value)       ((src) = BIT64_GET_MDF(src, n, value))
#endif

#if !defined(BIT64_FLIP)
/**
 * @brief 翻转 src 的 bit64_n.
 *
 * @param src 被翻转的变量.
 * @param n 比特下标, 从 0 开始.
 */
#   define BIT64_FLIP(src, n)             ((src) = BIT64_GET_FLIP(src, n))
#endif

#if !defined(BIT64_GET_LSH)
/**
 * @brief 获取左移 src 1 位的结果.
 *
 * @param src 被左移的变量.
 *
 * @return 左移 src 1 位 后的结果.
 */
#   define BIT64_GET_LSH(src)             ((src) << 1)
#endif

#if !defined(BIT64_GET_RSH)
/**
 * @brief 获取右移 src 1 位的结果.
 *
 * @param src 被右移的变量.
 *
 * @return 右移 src 1 位 后的结果.
 */
#   define BIT64_GET_RSH(src)             ((src) >> 1)
#endif

#if !defined(BIT64_MASK)
/**
 * @brief 获取低 n 位为 1 的位掩码.
 *
 * @param n 小于等于 32 的数.
 *
 * @return 低 n 位为 1 的位掩码.
 *
 * @code{markdown}
 * BIT64_MASK(6)  == 0x003f ==          0b00111111
 * BIT64_MASK(13) == 0x1fff == 0b00011111 11111111
 * @endcode
 */
#   define BIT64_MASK(n)                 (((n) < (8 * sizeof(BIT64_IE))) ? (BIT(n) - 1) : ~(BIT64_IE - 1))
#endif

#if !defined(BIT64_MASK_LSH)
/**
 * @brief 获取低 n 位为 1 的位掩码, 并左移 offset 位.
 *
 * @param n 小于等于 64 的数.
 * @param offset 左移位数.
 *
 * @return 高 n 位为 1 的位掩码.
 */
#   define BIT64_MASK_LSH(n, offset)      (BIT64_MASK(n) << (offset))
#endif

#if !defined(BITS64_GET_MDF0)
/**
 * @brief 获取设置 src 的对应位掩码 bitmask 为 1 的地方为 0 后的值.
 *
 * @param src 被设置的变量.
 * @param bitmask 位掩码. 注意! 为 1 的位表示需要被设为 0 的位.
 *
 * @note
 * 1. 位掩码为 1 的位表示需要被设为 0 的位.
 */
#   define BITS64_GET_MDF0(src, bitmask)  ((src) & ~(bitmask))
#endif

#if !defined(BITS64_GET_MDF1)
/**
 * @brief 获取设置 src 的对应位掩码 bitmask 为 1 的地方为 1 后的值.
 *
 * @param src 被设置的变量.
 * @param bitmask 位掩码. 注意! 为 1 的位表示需要被设为 1 的位.
 *
 * @note
 * 1. 位掩码为 1 的位表示需要被设为 1 的位.
 */
#   define BITS64_GET_MDF1(src, bitmask)  ((src) | (bitmask))
#endif

#if !defined(BITS64_GET_MDF_FLIP)
/**
 * @brief 获取翻转 src 的 bitmask 为 1 的位后的值.
 *
 * @param src 被设置的变量.
 * @param bitmask 位掩码. 注意! 为 1 的位表示需要被翻转的位.
 *
 * @note
 * 1. 位掩码为 1 的位表示需要被翻转的位.
 */
#   define BITS64_GET_MDF_FLIP(src, bitmask) \
                                        ((src) ^ (bitmask))
#endif

#if !defined(BITS64_SET0)
/**
 * @brief 设置 src 的对应位掩码 bitmask 为 1 的地方为 0.
 *
 * @param src 被设置的变量.
 * @param bitmask 位掩码. 注意! 为 1 的位表示需要被设为 0 的位.
 *
 * @note
 * 1. 位掩码为 1 的位表示需要被设为 0 的位.
 *
 * @code{markdown}
 * uint32_t src = 0xffff;
 * BITS64_SET0(src, 0b0011 0110 0101 1010) == 0b11001001 10100101
 * src = 0xffff;
 * BITS64_SET0(src, BIT64_MASK(4) << 3)     == 0b11111111 10000111
 * @endcode
 */
#   define BITS64_SET0(src, bitmask)      ((src) = BITS64_GET_MDF0(src, bitmask))
#endif

#if !defined(BITS64_SET1)
/**
 * @brief 设置 src 的对应位掩码 bitmask 为 1 的地方为 1.
 *
 * @param src 被设置的变量.
 * @param bitmask 位掩码. 注意! 为 1 的位表示需要被设为 1 的位.
 *
 * @note
 * 1. 位掩码为 1 的位表示需要被设为 1 的位.
 *
 * @code{markdown}
 * uint32_t src = 0x0000;
 * BITS64_SET1(src, 0b0011 0110 0101 1010) == 0b00110110 01011010
 * src = 0x0000;
 * BITS64_SET1(src, BIT64_MASK(4) << 3)     == 0b00000000 01111000
 * @endcode
 */
#   define BITS64_SET1(src, bitmask)      ((src) = BITS64_GET_MDF1(src, bitmask))
#endif

#if !defined(BITS64_FLIP)
/**
 * @brief 翻转 src 的 bitmask 为 1 的位.
 *
 * @param src 被设置的变量.
 * @param bitmask 位掩码. 注意! 为 1 的位表示需要被翻转的位.
 *
 * @note
 * 1. 位掩码为 1 的位表示需要被翻转的位.
 */
#   define BITS64_FLIP(src, bitmask)      ((src) = BITS64_GET_MDF_FLIP(src, bitmask))
#endif

#if !defined(BITSn64_GET_LSH)
/**
 * @brief 获取 src 保留低 n 位(共有 n 位)并左移 offset 位后的值.
 *
 * left shifts.
 *
 * @param src 被读取的源.
 * @param n 需要读取 n 位.
 * @param offset 偏移量. 从 bit0 起计算.
 *
 * @return src 内, 从 offset 位起, 共 n 位数据.
 */
#   define BITSn64_GET_LSH(src, n, offset) \
                                        (((src) & BIT64_MASK(n)) << (offset))
#endif

#if !defined(BITSn64_GET_RSH)
/**
 * @brief 获取 src 的从 offset 位起共 n 位数据.
 *
 * right shifts.
 *
 * @param src 被读取的源.
 * @param n 需要保留 n 位.
 * @param offset 偏移量. 从 bit0 起计算.
 *
 * @return src 内, 从 offset 位起, 共 n 位数据.
 *
 * @code{markdown}
 * BITS64_GET_RSH(0b0011 1010 1111 0010, 7, 5) == 0x57 == 0b01010111
 *
 * bit:                54 3210  : bit == 5
 * src:  0b0011 1010 1111 0010  : src == 0x3af2
 *              ↓      ↓
 * num:         7654 321        : n   == 7
 * ————————————————————————————————————————————
 * ret:     0b0 1010 111        : ret == 0x57
 * @endcode
 */
#   define BITSn64_GET_RSH(src, n, offset) \
                                        (((src) >> (offset)) & BIT64_MASK(n))
#endif

#if !defined(BITSn64_GET_MDF)
/**
 * @brief 获取将 src 的 offset 位起共 n 位修改为 value 后的值.
 *
 * @param src 被读取的源.
 * @param n 需要修改的位数.
 * @param offset 偏移量. 从 bit0 起计算.
 * @param value src 的 offset 位起 n 位源需要修改到的目标值.
 *
 * @return src 的 offset 位起 n 位修改为 value 的值.
 *
 * @code{markdown}
 *  BITSn64_GET_MDF(src       , n, offset, value     )
 *  BITSn64_GET_MDF(0x4c5ca6d2, 8, 13    , 0b10101111)
 *
 *  src:    0100 1100 0101 1100 1010 0110 1101 0010 == 0x4c5ca6d2
 *                       1 0101 111                 == ((value & BIT64_MASK(8)) << 13)
 *                       ↓ ↓↓↓↓ ↓↓↓
 *  ret:    0100 1100 0101 0101 1110 0110 1101 0010 == 0x4c55e6d2
 * @endcode
 *
 * @note
 * 1. n + offset 不要大于 32 位（如果没有设 BIT64_IE 为 1ULL）.
 */
#   define BITSn64_GET_MDF(src, n, offset, value) \
                                        (BITS64_GET_MDF0((src), BIT64_MASK_LSH((n), (offset))) \
                                            | BITSn64_GET_LSH((value), (n), (offset)))
#endif

#if !defined(BITSn64_SET)
/**
 * @brief 设置 src 的 offset 位起共 n 位为 value.
 *
 * @see BITSn64_GET_MDF @ref BITSn64_GET_MDF
 * @note
 * 1. n + offset 不要大于 32 位（如果没有设 BIT64_IE 为 1ULL）.
 */
#   define BITSn64_SET(src, n, offset, value) \
                                        ((src) = BITSn64_GET_MDF((src), (n), (offset), (value)))
#endif

#endif /* XF_COMMON_ENABLE_64BITS */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* __XF_BIT_H__ */
