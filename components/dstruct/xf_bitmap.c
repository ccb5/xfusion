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

/* ==================== [Includes] ========================================== */

#include "xf_bitmap.h"

/* ==================== [Defines] =========================================== */

/* ==================== [Typedefs] ========================================== */

/* ==================== [Static Prototypes] ================================= */

/* ==================== [Static Variables] ================================== */

/* ==================== [Macros] ============================================ */

/* ==================== [Global Functions] ================================== */

int32_t xf_bitmap32_ffs(const xf_bitmap32_t *p_bm, uint32_t bit_size)
{
    uint32_t blk_cnt;
    int32_t last_blk;
    int32_t blk_idx;
    xf_bitmap32_t bm_blk;
    uint32_t ffs_bit;
    if ((!p_bm) || (!bit_size)) {
        return -1;
    }
    blk_cnt = xf_bitmap_div_round_up(bit_size, XF_BITMAP32_BLK_BIT_SIZE);
    last_blk = blk_cnt;
    if (!IS_ALIGNED(bit_size, XF_BITMAP32_BLK_BIT_SIZE)) {
        --last_blk;
    }
    for (blk_idx = 0; blk_idx < last_blk; ++blk_idx) {
        bm_blk = p_bm[blk_idx];
        if (bm_blk) {
            ffs_bit = xf_am_ctz_u32(bm_blk);
            return (int32_t)(ffs_bit + (blk_idx * XF_BITMAP32_BLK_BIT_SIZE));
        }
    }
    if (last_blk < blk_cnt) {
        uint32_t valid;
        uint32_t mask;
        /* 最后一块，屏蔽无效位 */
        valid = bit_size % XF_BITMAP32_BLK_BIT_SIZE;
        mask  = BIT_MASK(valid);
        bm_blk = p_bm[last_blk] & mask;
        if (bm_blk) {
            ffs_bit = xf_am_ctz_u32(bm_blk);
            return (int32_t)(ffs_bit + (last_blk * XF_BITMAP32_BLK_BIT_SIZE));
        }
    }
    return -1;
}

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
        uint32_t invalid_mask;
        /* 屏蔽无效位 */
        valid_bit_num = bit_size % XF_BITMAP32_BLK_BIT_SIZE;
        invalid_mask = ~BIT_MASK(valid_bit_num);
        bm_blk = p_bm[blk_idx];
        BITS_SET0(bm_blk, invalid_mask);
        /* 1 和 0 log2 结果都为 0 */
        fls_bit = xf_am_log2_u32((uint32_t)bm_blk);
        if ((fls_bit != 0) || (bm_blk == 1)) {
            return fls_bit + (blk_idx * XF_BITMAP32_BLK_BIT_SIZE);
        }
    }
    /* 最高块为 0 或已对齐，找低位块 */
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

int32_t xf_bitmap32_ffz(const xf_bitmap32_t *p_bm, uint32_t bit_size)
{
    uint32_t blk_cnt;
    int32_t last_blk;
    int32_t blk_idx;
    xf_bitmap32_t bm_blk;
    xf_bitmap32_t zm_blk;
    uint32_t ffz_bit;
    if ((!p_bm) || (!bit_size)) {
        return -1;
    }
    blk_cnt = xf_bitmap_div_round_up(bit_size, XF_BITMAP32_BLK_BIT_SIZE);
    last_blk = blk_cnt;
    if (!IS_ALIGNED(bit_size, XF_BITMAP32_BLK_BIT_SIZE)) {
        --last_blk;
    }
    for (blk_idx = 0; blk_idx < last_blk; ++blk_idx) {
        bm_blk = p_bm[blk_idx];
        zm_blk = ~bm_blk;
        if (zm_blk) {
            ffz_bit = xf_am_ctz_u32(zm_blk);
            return (int32_t)(ffz_bit + (blk_idx * XF_BITMAP32_BLK_BIT_SIZE));
        }
    }
    if (last_blk < blk_cnt) {
        uint32_t valid;
        uint32_t mask;
        valid = bit_size % XF_BITMAP32_BLK_BIT_SIZE;
        mask = BIT_MASK(valid);
        bm_blk = p_bm[last_blk];
        zm_blk = (~bm_blk) & mask;
        if (zm_blk) {
            ffz_bit = xf_am_ctz_u32(zm_blk);
            return (int32_t)(ffz_bit + (last_blk * XF_BITMAP32_BLK_BIT_SIZE));
        }
    }
    return -1;
}
int32_t xf_bitmap32_flz(const xf_bitmap32_t *p_bm, uint32_t bit_size)
{
    int32_t blk_idx;
    xf_bitmap32_t bm_blk;
    xf_bitmap32_t zm_blk;
    uint32_t flz_bit;
    if ((!p_bm) || (!bit_size)) {
        return -1;
    }
    blk_idx = xf_div_round_down(bit_size, XF_BITMAP32_BLK_BIT_SIZE);
    if (!IS_ALIGNED(bit_size, XF_BITMAP32_BLK_BIT_SIZE)) {
        int32_t valid_bit_num;
        uint32_t invalid_mask;
        valid_bit_num = bit_size % XF_BITMAP32_BLK_BIT_SIZE;
        invalid_mask = ~BIT_MASK(valid_bit_num);
        bm_blk = p_bm[blk_idx];
        BITS_SET1(bm_blk, invalid_mask); /* 设为1以屏蔽无效位 */
        zm_blk = ~bm_blk;
        flz_bit = xf_am_log2_u32((uint32_t)zm_blk);
        if ((flz_bit != 0) || (zm_blk == 1)) {
            return flz_bit + (blk_idx * XF_BITMAP32_BLK_BIT_SIZE);
        }
    }
    --blk_idx;
    for (; blk_idx >= 0; --blk_idx) {
        bm_blk = p_bm[blk_idx];
        zm_blk = ~bm_blk;
        flz_bit = xf_am_log2_u32((uint32_t)zm_blk);
        if ((flz_bit != 0) || (zm_blk == 1)) {
            return flz_bit + (blk_idx * XF_BITMAP32_BLK_BIT_SIZE);
        }
    }
    return -1;
}

/* ==================== [Static Functions] ================================== */

#if 0
// 测试用例：
void TEST_CASE(void)
{
    {
        int32_t fls;
        XF_BITMAP32_DECLARE(bm, 64) = {0};
        XF_BITMAP32_SET1(bm, 55);
        for (int i = 63; i >= 0; i--) {
            XF_BITMAP32_SET1(bm, i);
            fls = xf_bitmap32_fls(bm, 64);
            XF_LOGI(TAG, "fls: %d", fls);
            if (i != 55) {
                XF_BITMAP32_SET0(bm, i);
            }
        }
        fls = xf_bitmap32_fls(bm, 64);
        XF_LOGI(TAG, "fls: %d", fls);
    }
    {
        int32_t fls;
        XF_BITMAP32_DECLARE(bm, 56) = {0};
        XF_BITMAP32_SET1(bm, 57);
        for (int i = 55; i >= 0; i--) {
            XF_BITMAP32_SET1(bm, i);
            fls = xf_bitmap32_fls(bm, 56);
            XF_LOGI(TAG, "fls: %d", fls);
            XF_BITMAP32_SET0(bm, i);
        }
        fls = xf_bitmap32_fls(bm, 56);
        XF_LOGI(TAG, "fls: %d", fls);
    }
    {
        int32_t fls;
        XF_BITMAP32_DECLARE(bm, 32) = {0};
        for (int i = 31; i >= 0; i--) {
            XF_BITMAP32_SET1(bm, i);
            fls = xf_bitmap32_fls(bm, 32);
            XF_LOGI(TAG, "fls: %d", fls);
            XF_BITMAP32_SET0(bm, i);
        }
        fls = xf_bitmap32_fls(bm, 32);
        XF_LOGI(TAG, "fls: %d", fls);
    }
    {
        int32_t fls;
        XF_BITMAP32_DECLARE(bm, 11) = {0};
        XF_BITMAP32_SET1(bm, 11);
        for (int i = 10; i >= 0; i--) {
            XF_BITMAP32_SET1(bm, i);
            fls = xf_bitmap32_fls(bm, 11);
            XF_LOGI(TAG, "fls: %d", fls);
            XF_BITMAP32_SET0(bm, i);
        }
        fls = xf_bitmap32_fls(bm, 11);
        XF_LOGI(TAG, "fls: %d", fls);
    }
    {
        int32_t fls;
        XF_BITMAP32_DECLARE(bm, 64) = {0};
        XF_BITMAP32_SET1(bm, 31);
        for (int i = 64; i >= 0; i--) {
            fls = xf_bitmap32_fls(bm, i);
            XF_LOGI(TAG, "i: %d, fls: %d", i, fls);
        }
    }

    {
        int32_t ffs;
        XF_BITMAP32_DECLARE(bm, 64) = {0};
        XF_BITMAP32_SET1(bm, 55);
        for (int i = 0; i < 64; i++) {
            XF_BITMAP32_SET1(bm, i);
            ffs = xf_bitmap32_ffs(bm, 64);
            XF_LOGI(TAG, "ffs: %d", ffs);
            if (i != 55) {
                XF_BITMAP32_SET0(bm, i);
            }
        }
        ffs = xf_bitmap32_ffs(bm, 64);
        XF_LOGI(TAG, "ffs: %d", ffs);
    }
    {
        int32_t ffs;
        XF_BITMAP32_DECLARE(bm, 11) = {0};
        for (int i = 0; i < 11; i++) {
            XF_BITMAP32_SET1(bm, i);
            ffs = xf_bitmap32_ffs(bm, 11);
            XF_LOGI(TAG, "ffs: %d", ffs);
            XF_BITMAP32_SET0(bm, i);
        }
        ffs = xf_bitmap32_ffs(bm, 11);
        XF_LOGI(TAG, "ffs: %d", ffs);
    }
    {
        int32_t ffs;
        XF_BITMAP32_DECLARE(bm, 64) = {0};
        XF_BITMAP32_SET1(bm, 31);
        XF_BITMAP32_SET1(bm, 55);
        for (int i = 0; i < 64; i++) {
            ffs = xf_bitmap32_ffs(bm, i + 1);
            XF_LOGI(TAG, "i: %d, ffs: %d", i, ffs);
        }
    }

    {
        int32_t ffz;
        XF_BITMAP32_DECLARE(bm, 64) = {0xFFFFFFFF, 0xFFFFFFFF};
        XF_BITMAP32_SET0(bm, 55);
        for (int i = 0; i < 64; i++) {
            XF_BITMAP32_SET0(bm, i);
            ffz = xf_bitmap32_ffz(bm, 64);
            XF_LOGI(TAG, "ffz: %d", ffz);
            if (i != 55) {
                XF_BITMAP32_SET1(bm, i);
            }
        }
        ffz = xf_bitmap32_ffz(bm, 64);
        XF_LOGI(TAG, "ffz: %d", ffz);
    }
    {
        int32_t ffz;
        XF_BITMAP32_DECLARE(bm, 11) = {0xFFFFFFFF};
        for (int i = 0; i < 11; i++) {
            XF_BITMAP32_SET0(bm, i);
            ffz = xf_bitmap32_ffz(bm, 11);
            XF_LOGI(TAG, "ffz: %d", ffz);
            XF_BITMAP32_SET1(bm, i);
        }
        ffz = xf_bitmap32_ffz(bm, 11);
        XF_LOGI(TAG, "ffz: %d", ffz);
    }
    {
        int32_t ffz;
        XF_BITMAP32_DECLARE(bm, 64) = {0xFFFFFFFF, 0xFFFFFFFF};
        XF_BITMAP32_SET0(bm, 31);
        XF_BITMAP32_SET0(bm, 55);
        for (int i = 0; i < 64; i++) {
            ffz = xf_bitmap32_ffz(bm, i + 1);
            XF_LOGI(TAG, "i: %d, ffz: %d", i, ffz);
        }
    }

    {
        int32_t flz;
        XF_BITMAP32_DECLARE(bm, 64) = {0xFFFFFFFF, 0xFFFFFFFF};
        XF_BITMAP32_SET0(bm, 5);
        for (int i = 63; i >= 0; i--) {
            XF_BITMAP32_SET0(bm, i);
            flz = xf_bitmap32_flz(bm, 64);
            XF_LOGI(TAG, "flz: %d", flz);
            if (i != 5) {
                XF_BITMAP32_SET1(bm, i);
            }
        }
        flz = xf_bitmap32_flz(bm, 64);
        XF_LOGI(TAG, "flz: %d", flz);
    }
    {
        int32_t flz;
        XF_BITMAP32_DECLARE(bm, 11) = {0xFFFFFFFF};
        XF_BITMAP32_SET0(bm, 11);
        for (int i = 10; i >= 0; i--) {
            XF_BITMAP32_SET0(bm, i);
            flz = xf_bitmap32_flz(bm, 11);
            XF_LOGI(TAG, "flz: %d", flz);
            XF_BITMAP32_SET1(bm, i);
        }
        flz = xf_bitmap32_flz(bm, 11);
        XF_LOGI(TAG, "flz: %d", flz);
    }
    {
        int32_t flz;
        XF_BITMAP32_DECLARE(bm, 64) = {0xFFFFFFFF, 0xFFFFFFFF};
        XF_BITMAP32_SET0(bm, 31);
        for (int i = 64; i >= 0; i--) {
            flz = xf_bitmap32_flz(bm, i);
            XF_LOGI(TAG, "i: %d, flz: %d", i, flz);
        }
    }
}
#endif
