/**
 * @file xf_event.c
 * @author catcatBlue (catcatblue@qq.com)
 * @brief
 * @version 1.0
 * @date 2025-05-22
 *
 * SPDX-FileCopyrightText: 2025 CompanyNameMagicTag
 * SPDX-License-Identifier: Apache-2.0
 *
 */

/* ==================== [Includes] ========================================== */

#include "xf_event.h"

/* ==================== [Defines] =========================================== */

/* ==================== [Typedefs] ========================================== */

typedef xf_bitmap_t xf_event_bitmap_t;

/* ==================== [Static Prototypes] ================================= */

/* ==================== [Static Variables] ================================== */

/* 唯一事件 id 位图 */
static xf_event_bitmap_t s_eid_bm[XF_BITMAP32_GET_BLK_SIZE(XF_EVENT_ID_NUM_MAX)];

/* ==================== [Macros] ============================================ */

/* ==================== [Global Functions] ================================== */

xf_event_id_t xf_event_acquire_id(void)
{
    int32_t idx;
    idx = xf_bitmap32_ffz(s_eid_bm, XF_EVENT_ID_NUM_MAX);
    if (idx < 0) {
        return XF_EVENT_ID_INVALID;
    }
    XF_BITMAP32_SET1(s_eid_bm, idx);
    return idx + XF_EVENT_ID_OFFSET;
}

xf_err_t xf_event_release_id(xf_event_id_t id)
{
    if ((id < XF_EVENT_ID_OFFSET)
            || (id > (XF_EVENT_ID_OFFSET + XF_EVENT_ID_NUM_MAX - 1U))) {
        return XF_FAIL;
    }
    id -= (xf_event_id_t)XF_EVENT_ID_OFFSET;
    if (XF_BITMAP_GET(s_eid_bm, id) == 0) {
        return XF_FAIL;
    }
    XF_BITMAP_SET0(s_eid_bm, id);
    return XF_OK;
}

/* ==================== [Static Functions] ================================== */
