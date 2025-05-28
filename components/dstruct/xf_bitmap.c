/**
 * @file xf_bitmap.c
 * @author catcatBlue (catcatblue@qq.com)
 * @brief
 * @version 1.0
 * @date 2025-05-28
 *
 * SPDX-FileCopyrightText: 2025 CompanyNameMagicTag
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#if 0
// 测试用例：
// @code{c}
// {
//     int32_t fls;
//     XF_BITMAP32_DECLARE(bm, 64) = {0};
//     for (int i = 63; i >= 0; i--) {
//         XF_BITMAP32_SET1(bm, i);
//         fls = xf_bitmap32_fls(bm, 64);
//         XF_LOGI(TAG, "fls: %d", fls);
//         XF_BITMAP32_SET0(bm, i);
//     }
//     fls = xf_bitmap32_fls(bm, 64);
//     XF_LOGI(TAG, "fls: %d", fls);
// }
// {
//     int32_t fls;
//     XF_BITMAP32_DECLARE(bm, 56) = {0};
//     XF_BITMAP32_SET1(bm, 57);
//     for (int i = 55; i >= 0; i--) {
//         XF_BITMAP32_SET1(bm, i);
//         fls = xf_bitmap32_fls(bm, 56);
//         XF_LOGI(TAG, "fls: %d", fls);
//         XF_BITMAP32_SET0(bm, i);
//     }
//     fls = xf_bitmap32_fls(bm, 56);
//     XF_LOGI(TAG, "fls: %d", fls);
// }
// {
//     int32_t fls;
//     XF_BITMAP32_DECLARE(bm, 32) = {0};
//     for (int i = 31; i >= 0; i--) {
//         XF_BITMAP32_SET1(bm, i);
//         fls = xf_bitmap32_fls(bm, 32);
//         XF_LOGI(TAG, "fls: %d", fls);
//         XF_BITMAP32_SET0(bm, i);
//     }
//     fls = xf_bitmap32_fls(bm, 32);
//     XF_LOGI(TAG, "fls: %d", fls);
// }
// {
//     int32_t fls;
//     XF_BITMAP32_DECLARE(bm, 11) = {0};
//     XF_BITMAP32_SET1(bm, 11);
//     for (int i = 10; i >= 0; i--) {
//         XF_BITMAP32_SET1(bm, i);
//         fls = xf_bitmap32_fls(bm, 11);
//         XF_LOGI(TAG, "fls: %d", fls);
//         XF_BITMAP32_SET0(bm, i);
//     }
//     fls = xf_bitmap32_fls(bm, 11);
//     XF_LOGI(TAG, "fls: %d", fls);
// }
// @endcode
#endif

/* ==================== [Includes] ========================================== */

#include "xf_bitmap.h"

/* ==================== [Defines] =========================================== */

/* ==================== [Typedefs] ========================================== */

/* ==================== [Static Prototypes] ================================= */

/* ==================== [Static Variables] ================================== */

/* ==================== [Macros] ============================================ */

/* ==================== [Global Functions] ================================== */

int32_t xf_bitmap32_fls(const xf_bitmap32_t *p_bm, uint32_t bit_size)
{
    int32_t blk_idx;
    xf_bitmap32_t bm_blk;
    uint32_t fls_bit;
    if ((!p_bm) || (!bit_size)) {
        return -1;
    }
    blk_idx = xf_div_round_down(bit_size, XF_BITMAP32_BLK_BIT_SIZE);
    if (!IS_ALIGNED(bit_size, XF_BITMAP32_BLK_BIT_SIZE)) {
        int32_t valid_bit_num;
        int32_t invalid_bit_num;
        uint32_t invalid_mask;
        /* 屏蔽无效位 */
        valid_bit_num = bit_size % XF_BITMAP32_BLK_BIT_SIZE;
        invalid_bit_num = XF_BITMAP32_BLK_BIT_SIZE - valid_bit_num;
        invalid_mask = BIT_MASK_LSH(invalid_bit_num, valid_bit_num);
        bm_blk = p_bm[blk_idx];
        BITS_SET0(bm_blk, invalid_mask);
        /* 1 和 0 log2 结果都为 0 */
        fls_bit = xf_am_log2_u32((uint32_t)bm_blk);
        if ((fls_bit != 0) || (bm_blk == 1)) {
            return fls_bit + (blk_idx * XF_BITMAP32_BLK_BIT_SIZE);
        }
    }
    /* 最高块为 0, 找低位块 */
    --blk_idx;
    for (; blk_idx >= 0; --blk_idx) {
        bm_blk = p_bm[blk_idx];
        fls_bit = xf_am_log2_u32((uint32_t)bm_blk);
        if ((fls_bit != 0) || (bm_blk == 1)) {
            return fls_bit + (blk_idx * XF_BITMAP32_BLK_BIT_SIZE);
        }
    }
    return -1;
}

/* ==================== [Static Functions] ================================== */
