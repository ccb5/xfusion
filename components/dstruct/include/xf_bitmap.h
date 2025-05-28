/**
 * @file xf_bitmap.h
 * @author catcatBlue (catcatblue@qq.com)
 * @brief
 * @version 1.0
 * @date 2025-05-20
 *
 * SPDX-FileCopyrightText: 2025 CompanyNameMagicTag
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#ifndef __XF_BITMAP_H__
#define __XF_BITMAP_H__

/* ==================== [Includes] ========================================== */

#include "xf_common.h"
#include "xf_algo.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== [Defines] =========================================== */

#if !defined(XF_BITMAP_BLK_SIZE)
#   define XF_BITMAP_BLK_SIZE 32
#endif

#if defined(XF_BITMAP_BLK_SIZE) \
        && ((XF_BITMAP_BLK_SIZE != 8) \
            && (XF_BITMAP_BLK_SIZE != 16) \
            && (XF_BITMAP_BLK_SIZE != 32) \
            && (XF_BITMAP_BLK_SIZE != 64))
#   error "XF_BITMAP_BLK_SIZE must be 8, 16, 32, 64"
#endif

/* ==================== [Typedefs] ========================================== */

typedef uint8_t     xf_bitmap8_t;
typedef uint16_t    xf_bitmap16_t;
typedef uint32_t    xf_bitmap32_t;
typedef uint64_t    xf_bitmap64_t;

/* ==================== [Global Prototypes] ================================= */

/*
    ffs:    Find First Set, 从低到高找第一个 1 出现的位置
    fls:    Find Last Set, 从低到高找最后一个 1 出现的位置，
            也就是从高到低找第一个 1 出现的位置。
 */

#if XF_TODO
int32_t xf_bitmap8_fls(const xf_bitmap8_t *p_bm, uint32_t bit_size);
int32_t xf_bitmap16_fls(const xf_bitmap16_t *p_bm, uint32_t bit_size);
int32_t xf_bitmap64_fls(const xf_bitmap64_t *p_bm, uint32_t bit_size);

int32_t xf_bitmap8_ffs(const xf_bitmap8_t *p_bm, uint32_t bit_size);
int32_t xf_bitmap16_ffs(const xf_bitmap16_t *p_bm, uint32_t bit_size);
int32_t xf_bitmap32_ffs(const xf_bitmap32_t *p_bm, uint32_t bit_size);
int32_t xf_bitmap64_ffs(const xf_bitmap64_t *p_bm, uint32_t bit_size);
#endif

int32_t xf_bitmap32_fls(const xf_bitmap32_t *p_bm, uint32_t bit_size);

/* ==================== [Macros] ============================================ */

#if !defined(xf_bitmap_div_round_up)
#   define xf_bitmap_div_round_up(n, d)                 (((n) + (d) - 1) / (d))
#endif

#define XF_BITMAP8_BLK_BIT_SIZE                         (8 * sizeof(xf_bitmap8_t))
#define XF_BITMAP8_GET_BLK_SIZE(_bit_size)              xf_bitmap_div_round_up((_bit_size), XF_BITMAP8_BLK_BIT_SIZE)
#define XF_BITMAP8_DECLARE(_bitmap, _bit_size)          xf_bitmap8_t _bitmap[XF_BITMAP8_GET_BLK_SIZE(_bit_size)]
#define XF_BITMAP8_GET_BLK_POS(_bit)                    ((_bit) / (sizeof(xf_bitmap8_t) * 8))
#define XF_BITMAP8_GET_BIT_POS_IN_BLK(_bit)             ((_bit) % (sizeof(xf_bitmap8_t) * 8))
#define XF_BITMAP8_GET_BLK(_bitmap, _bit)               (_bitmap[XF_BITMAP8_GET_BLK_POS(_bit)])
#define XF_BITMAP8_GET_BIT_POS(_bitmap, _bit)           ((xf_bitmap8_t)1 << XF_BITMAP8_GET_BIT_POS_IN_BLK(_bit))
#define XF_BITMAP8_GET(_bitmap, _bit)                   ((xf_bitmap8_t)!!(XF_BITMAP8_GET_BLK(_bitmap, (_bit)) & XF_BITMAP8_GET_BIT_POS(_bitmap, _bit)))
#define XF_BITMAP8_GET_MDF1(_bitmap, _bit)              (XF_BITMAP8_GET_BLK(_bitmap, _bit) | XF_BITMAP8_GET_BIT_POS(_bitmap, _bit))
#define XF_BITMAP8_GET_MDF0(_bitmap, _bit)              (XF_BITMAP8_GET_BLK(_bitmap, _bit) & ~XF_BITMAP8_GET_BIT_POS(_bitmap, _bit))
#define XF_BITMAP8_GET_MDF(_bitmap, _bit, _value)       (XF_BITMAP8_GET_MDF0(_bitmap, _bit) | ((xf_bitmap8_t)(!!(_value)) << XF_BITMAP8_GET_BIT_POS_IN_BLK(_bit)))
#define XF_BITMAP8_GET_MDF_FLIP(_bitmap, _bit)          (XF_BITMAP8_GET_BLK(_bitmap, _bit) ^ XF_BITMAP8_GET_BIT_POS(_bitmap, _bit))
#define XF_BITMAP8_SET1(_bitmap, _bit)                  (XF_BITMAP8_GET_BLK(_bitmap, _bit) = XF_BITMAP8_GET_MDF1(_bitmap, _bit))
#define XF_BITMAP8_SET0(_bitmap, _bit)                  (XF_BITMAP8_GET_BLK(_bitmap, _bit) = XF_BITMAP8_GET_MDF0(_bitmap, _bit))
#define XF_BITMAP8_SET(_bitmap, _bit, _value)           (XF_BITMAP8_GET_BLK(_bitmap, _bit) = XF_BITMAP8_GET_MDF(_bitmap, _bit, _value))
#define XF_BITMAP8_SET_FLIP(_bitmap, _bit)              (XF_BITMAP8_GET_BLK(_bitmap, _bit) = XF_BITMAP8_GET_MDF_FLIP(_bitmap, _bit))

#define XF_BITMAP16_BLK_BIT_SIZE                        (8 * sizeof(xf_bitmap16_t))
#define XF_BITMAP16_GET_BLK_SIZE(_bit_size)             xf_bitmap_div_round_up((_bit_size), XF_BITMAP8_BLK_BIT_SIZE)
#define XF_BITMAP16_DECLARE(_bitmap, _bit_size)         xf_bitmap16_t _bitmap[XF_BITMAP16_GET_BLK_SIZE(_bit_size)]
#define XF_BITMAP16_GET_BLK_POS(_bit)                   ((_bit) / (sizeof(xf_bitmap16_t) * 8))
#define XF_BITMAP16_GET_BIT_POS_IN_BLK(_bit)            ((_bit) % (sizeof(xf_bitmap16_t) * 8))
#define XF_BITMAP16_GET_BLK(_bitmap, _bit)              (_bitmap[XF_BITMAP16_GET_BLK_POS(_bit)])
#define XF_BITMAP16_GET_BIT_POS(_bitmap, _bit)          ((xf_bitmap16_t)1 << XF_BITMAP16_GET_BIT_POS_IN_BLK(_bit))
#define XF_BITMAP16_GET(_bitmap, _bit)                  ((xf_bitmap16_t)!!(XF_BITMAP16_GET_BLK(_bitmap, (_bit)) & XF_BITMAP16_GET_BIT_POS(_bitmap, _bit)))
#define XF_BITMAP16_GET_MDF1(_bitmap, _bit)             (XF_BITMAP16_GET_BLK(_bitmap, _bit) | XF_BITMAP16_GET_BIT_POS(_bitmap, _bit))
#define XF_BITMAP16_GET_MDF0(_bitmap, _bit)             (XF_BITMAP16_GET_BLK(_bitmap, _bit) & ~XF_BITMAP16_GET_BIT_POS(_bitmap, _bit))
#define XF_BITMAP16_GET_MDF(_bitmap, _bit, _value)      (XF_BITMAP16_GET_MDF0(_bitmap, _bit) | ((xf_bitmap16_t)(!!(_value)) << XF_BITMAP16_GET_BIT_POS_IN_BLK(_bit)))
#define XF_BITMAP16_GET_MDF_FLIP(_bitmap, _bit)         (XF_BITMAP16_GET_BLK(_bitmap, _bit) ^ XF_BITMAP16_GET_BIT_POS(_bitmap, _bit))
#define XF_BITMAP16_SET1(_bitmap, _bit)                 (XF_BITMAP16_GET_BLK(_bitmap, _bit) = XF_BITMAP16_GET_MDF1(_bitmap, _bit))
#define XF_BITMAP16_SET0(_bitmap, _bit)                 (XF_BITMAP16_GET_BLK(_bitmap, _bit) = XF_BITMAP16_GET_MDF0(_bitmap, _bit))
#define XF_BITMAP16_SET(_bitmap, _bit, _value)          (XF_BITMAP16_GET_BLK(_bitmap, _bit) = XF_BITMAP16_GET_MDF(_bitmap, _bit, _value))
#define XF_BITMAP16_SET_FLIP(_bitmap, _bit)             (XF_BITMAP16_GET_BLK(_bitmap, _bit) = XF_BITMAP16_GET_MDF_FLIP(_bitmap, _bit))

#define XF_BITMAP32_BLK_BIT_SIZE                        (8 * sizeof(xf_bitmap32_t))
#define XF_BITMAP32_GET_BLK_SIZE(_bit_size)             xf_bitmap_div_round_up((_bit_size), XF_BITMAP32_BLK_BIT_SIZE)
#define XF_BITMAP32_DECLARE(_bitmap, _bit_size)         xf_bitmap32_t _bitmap[XF_BITMAP32_GET_BLK_SIZE(_bit_size)]
#define XF_BITMAP32_GET_BLK_POS(_bit)                   ((_bit) / (sizeof(xf_bitmap32_t) * 8))
#define XF_BITMAP32_GET_BIT_POS_IN_BLK(_bit)            ((_bit) % (sizeof(xf_bitmap32_t) * 8))
#define XF_BITMAP32_GET_BLK(_bitmap, _bit)              (_bitmap[XF_BITMAP32_GET_BLK_POS(_bit)])
#define XF_BITMAP32_GET_BIT_POS(_bitmap, _bit)          ((xf_bitmap32_t)1 << XF_BITMAP32_GET_BIT_POS_IN_BLK(_bit))
#define XF_BITMAP32_GET(_bitmap, _bit)                  ((xf_bitmap32_t)!!(XF_BITMAP32_GET_BLK(_bitmap, (_bit)) & XF_BITMAP32_GET_BIT_POS(_bitmap, _bit)))
#define XF_BITMAP32_GET_MDF1(_bitmap, _bit)             (XF_BITMAP32_GET_BLK(_bitmap, _bit) | XF_BITMAP32_GET_BIT_POS(_bitmap, _bit))
#define XF_BITMAP32_GET_MDF0(_bitmap, _bit)             (XF_BITMAP32_GET_BLK(_bitmap, _bit) & ~XF_BITMAP32_GET_BIT_POS(_bitmap, _bit))
#define XF_BITMAP32_GET_MDF(_bitmap, _bit, _value)      (XF_BITMAP32_GET_MDF0(_bitmap, _bit) | ((xf_bitmap32_t)(!!(_value)) << XF_BITMAP32_GET_BIT_POS_IN_BLK(_bit)))
#define XF_BITMAP32_GET_MDF_FLIP(_bitmap, _bit)         (XF_BITMAP32_GET_BLK(_bitmap, _bit) ^ XF_BITMAP32_GET_BIT_POS(_bitmap, _bit))
#define XF_BITMAP32_SET1(_bitmap, _bit)                 (XF_BITMAP32_GET_BLK(_bitmap, _bit) = XF_BITMAP32_GET_MDF1(_bitmap, _bit))
#define XF_BITMAP32_SET0(_bitmap, _bit)                 (XF_BITMAP32_GET_BLK(_bitmap, _bit) = XF_BITMAP32_GET_MDF0(_bitmap, _bit))
#define XF_BITMAP32_SET(_bitmap, _bit, _value)          (XF_BITMAP32_GET_BLK(_bitmap, _bit) = XF_BITMAP32_GET_MDF(_bitmap, _bit, _value))
#define XF_BITMAP32_SET_FLIP(_bitmap, _bit)             (XF_BITMAP32_GET_BLK(_bitmap, _bit) = XF_BITMAP32_GET_MDF_FLIP(_bitmap, _bit))

#define XF_BITMAP64_BLK_BIT_SIZE                        (8 * sizeof(xf_bitmap64_t))
#define XF_BITMAP64_GET_BLK_SIZE(_bit_size)             xf_bitmap_div_round_up((_bit_size), XF_BITMAP64_BLK_BIT_SIZE)
#define XF_BITMAP64_DECLARE(_bitmap, _bit_size)         xf_bitmap64_t _bitmap[XF_BITMAP64_GET_BLK_SIZE(_bit_size)]
#define XF_BITMAP64_GET_BLK_POS(_bit)                   ((_bit) / (sizeof(xf_bitmap64_t) * 8))
#define XF_BITMAP64_GET_BIT_POS_IN_BLK(_bit)            ((_bit) % (sizeof(xf_bitmap64_t) * 8))
#define XF_BITMAP64_GET_BLK(_bitmap, _bit)              (_bitmap[XF_BITMAP64_GET_BLK_POS(_bit)])
#define XF_BITMAP64_GET_BIT_POS(_bitmap, _bit)          ((xf_bitmap64_t)1 << XF_BITMAP64_GET_BIT_POS_IN_BLK(_bit))
#define XF_BITMAP64_GET(_bitmap, _bit)                  ((xf_bitmap64_t)!!(XF_BITMAP64_GET_BLK(_bitmap, (_bit)) & XF_BITMAP64_GET_BIT_POS(_bitmap, _bit)))
#define XF_BITMAP64_GET_MDF1(_bitmap, _bit)             (XF_BITMAP64_GET_BLK(_bitmap, _bit) | XF_BITMAP64_GET_BIT_POS(_bitmap, _bit))
#define XF_BITMAP64_GET_MDF0(_bitmap, _bit)             (XF_BITMAP64_GET_BLK(_bitmap, _bit) & ~XF_BITMAP64_GET_BIT_POS(_bitmap, _bit))
#define XF_BITMAP64_GET_MDF(_bitmap, _bit, _value)      (XF_BITMAP64_GET_MDF0(_bitmap, _bit) | ((xf_bitmap64_t)(!!(_value)) << XF_BITMAP64_GET_BIT_POS_IN_BLK(_bit)))
#define XF_BITMAP64_GET_MDF_FLIP(_bitmap, _bit)         (XF_BITMAP64_GET_BLK(_bitmap, _bit) ^ XF_BITMAP64_GET_BIT_POS(_bitmap, _bit))
#define XF_BITMAP64_SET1(_bitmap, _bit)                 (XF_BITMAP64_GET_BLK(_bitmap, _bit) = XF_BITMAP64_GET_MDF1(_bitmap, _bit))
#define XF_BITMAP64_SET0(_bitmap, _bit)                 (XF_BITMAP64_GET_BLK(_bitmap, _bit) = XF_BITMAP64_GET_MDF0(_bitmap, _bit))
#define XF_BITMAP64_SET(_bitmap, _bit, _value)          (XF_BITMAP64_GET_BLK(_bitmap, _bit) = XF_BITMAP64_GET_MDF(_bitmap, _bit, _value))
#define XF_BITMAP64_SET_FLIP(_bitmap, _bit)             (XF_BITMAP64_GET_BLK(_bitmap, _bit) = XF_BITMAP64_GET_MDF_FLIP(_bitmap, _bit))

typedef XCAT3(xf_bitmap, XF_BITMAP_BLK_SIZE, _t)        xf_bitmap_t;

#if 1
#define XF_BITMAP_BLK_BIT_SIZE                          XCAT3(XF_BITMAP, XF_BITMAP_BLK_SIZE, _BLK_BIT_SIZE)
#define XF_BITMAP_GET_BLK_SIZE(_bit_size)               XCAT3(XF_BITMAP, XF_BITMAP_BLK_SIZE, _GET_BLK_SIZE) (_bit_size)
#define XF_BITMAP_DECLARE(_bitmap, _bit_size)           XCAT3(XF_BITMAP, XF_BITMAP_BLK_SIZE, _DECLARE) (_bitmap, (_bit_size))
#define XF_BITMAP_GET_BLK_POS(_bit)                     XCAT3(XF_BITMAP, XF_BITMAP_BLK_SIZE, _GET_BLK_POS) ((_bit))
#define XF_BITMAP_GET_BIT_POS_IN_BLK(_bit)              XCAT3(XF_BITMAP, XF_BITMAP_BLK_SIZE, _GET_BIT_POS_IN_BLK) ((_bit))
#define XF_BITMAP_GET_BLK(_bitmap, _bit)                XCAT3(XF_BITMAP, XF_BITMAP_BLK_SIZE, _GET_BLK) ((_bitmap), (_bit))
#define XF_BITMAP_GET_BIT_POS(_bitmap, _bit)            XCAT3(XF_BITMAP, XF_BITMAP_BLK_SIZE, _GET_BIT_POS) ((_bitmap), (_bit))
#define XF_BITMAP_GET(_bitmap, _bit)                    XCAT3(XF_BITMAP, XF_BITMAP_BLK_SIZE, _GET) ((_bitmap), (_bit))
#define XF_BITMAP_GET_MDF1(_bitmap, _bit)               XCAT3(XF_BITMAP, XF_BITMAP_BLK_SIZE, _GET_MDF1) ((_bitmap), (_bit))
#define XF_BITMAP_GET_MDF0(_bitmap, _bit)               XCAT3(XF_BITMAP, XF_BITMAP_BLK_SIZE, _GET_MDF0) ((_bitmap), (_bit))
#define XF_BITMAP_GET_MDF(_bitmap, _bit, _value)        XCAT3(XF_BITMAP, XF_BITMAP_BLK_SIZE, _GET_MDF) ((_bitmap), (_bit), (_value))
#define XF_BITMAP_GET_MDF_FLIP(_bitmap, _bit)           XCAT3(XF_BITMAP, XF_BITMAP_BLK_SIZE, _MDF_FLIP) ((_bitmap), (_value))
#define XF_BITMAP_SET1(_bitmap, _bit)                   XCAT3(XF_BITMAP, XF_BITMAP_BLK_SIZE, _SET1) ((_bitmap), (_bit))
#define XF_BITMAP_SET0(_bitmap, _bit)                   XCAT3(XF_BITMAP, XF_BITMAP_BLK_SIZE, _SET0) ((_bitmap), (_bit))
#define XF_BITMAP_SET(_bitmap, _bit, _value)            XCAT3(XF_BITMAP, XF_BITMAP_BLK_SIZE, _SET) ((_bitmap), (_bit), (_value))
#define XF_BITMAP_SET_FLIP(_bitmap, _bit)               XCAT3(XF_BITMAP, XF_BITMAP_BLK_SIZE, _SET_FLIP) ((_bitmap), (_bit))
#else
#define XF_BITMAP_BLK_BIT_SIZE                          XCAT3(XF_BITMAP, XF_BITMAP_BLK_SIZE, _BLK_BIT_SIZE)
#define XF_BITMAP_GET_BLK_SIZE          XCAT3(XF_BITMAP, XF_BITMAP_BLK_SIZE, _GET_BLK_SIZE)
#define XF_BITMAP_DECLARE               XCAT3(XF_BITMAP, XF_BITMAP_BLK_SIZE, _DECLARE)
#define XF_BITMAP_GET_BLK_POS           XCAT3(XF_BITMAP, XF_BITMAP_BLK_SIZE, _GET_BLK_POS)
#define XF_BITMAP_GET_BIT_POS_IN_BLK    XCAT3(XF_BITMAP, XF_BITMAP_BLK_SIZE, _GET_BIT_POS_IN_BLK)
#define XF_BITMAP_GET_BLK               XCAT3(XF_BITMAP, XF_BITMAP_BLK_SIZE, _GET_BLK)
#define XF_BITMAP_GET_BIT_POS           XCAT3(XF_BITMAP, XF_BITMAP_BLK_SIZE, _GET_BIT_POS)
#define XF_BITMAP_GET                   XCAT3(XF_BITMAP, XF_BITMAP_BLK_SIZE, _GET)
#define XF_BITMAP_GET_MDF1              XCAT3(XF_BITMAP, XF_BITMAP_BLK_SIZE, _GET_MDF1)
#define XF_BITMAP_GET_MDF0              XCAT3(XF_BITMAP, XF_BITMAP_BLK_SIZE, _GET_MDF0)
#define XF_BITMAP_GET_MDF               XCAT3(XF_BITMAP, XF_BITMAP_BLK_SIZE, _GET_MDF)
#define XF_BITMAP_GET_MDF_FLIP          XCAT3(XF_BITMAP, XF_BITMAP_BLK_SIZE, _GET_MDF_FLIP)
#define XF_BITMAP_SET1                  XCAT3(XF_BITMAP, XF_BITMAP_BLK_SIZE, _SET1)
#define XF_BITMAP_SET0                  XCAT3(XF_BITMAP, XF_BITMAP_BLK_SIZE, _SET0)
#define XF_BITMAP_SET                   XCAT3(XF_BITMAP, XF_BITMAP_BLK_SIZE, _SET)
#define XF_BITMAP_SET_FLIP              XCAT3(XF_BITMAP, XF_BITMAP_BLK_SIZE, _SET_FLIP)
#endif

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* __XF_BITMAP_H__ */
