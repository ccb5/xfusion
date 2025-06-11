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

/* ==================== [Static Prototypes] ================================= */

static xf_err_t xf_event_gc_cb_static(xf_event_t *const e);
static xf_err_t xf_event_gc_cb_dyn(xf_event_t *const e);

/* ==================== [Static Variables] ================================== */

static const char *const TAG = "xf_event";

/* 唯一事件 id 位图 */
static xf_event_bitmap_t s_eid_bm[XF_BITMAP32_GET_BLK_SIZE(XF_EVENT_ID_NUM_MAX)];

STATIC_ASSERT(XF_EVENT_GC_NUM_MAX <= 16);
static xf_event_gc_cb_t s_gc[XF_EVENT_GC_NUM_MAX] = {0};

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

xf_err_t xf_event_init(void)
{
    s_gc[XF_EVENT_POOL_ID_STATIC] = xf_event_gc_cb_static;
    s_gc[XF_EVENT_POOL_ID_DYN] = xf_event_gc_cb_dyn;
    return XF_OK;
}

xf_err_t xf_event_set_pool_id(xf_event_t *const e, xf_event_pool_id_t id)
{
    if (e == NULL) {
        return XF_FAIL;
    }
    if (id >= XF_EVENT_GC_NUM_MAX) {
        return XF_FAIL;
    }
    xf_event_attr_set_pool_id(e, id);
    return XF_OK;
}

xf_event_pool_id_t xf_event_get_pool_id(xf_event_t *const e)
{
    if (e == NULL) {
        return XF_FAIL;
    }
    return xf_event_attr_get_pool_id(e);
}

xf_err_t xf_event_gc(xf_event_t *const e)
{
    xf_event_pool_id_t id;
    if (e == NULL) {
        return XF_FAIL;
    }
    if (e->ref_cnt != 0) {
        XF_LOGD(TAG, "ref_cnt:%u", (unsigned int)e->ref_cnt);
        XF_ERROR_LINE();
        return XF_FAIL;
    }
    id = xf_event_attr_get_pool_id(e);
    if (id >= XF_EVENT_GC_NUM_MAX) {
        return XF_FAIL;
    }
    if (((id == XF_EVENT_POOL_ID_STATIC) || (id == XF_EVENT_POOL_ID_DYN))
            && (s_gc[id] == NULL)) {
        xf_event_init();
    }
    if (s_gc[id] == NULL) {
        return XF_FAIL;
    }
    return s_gc[id](e);
}

xf_err_t xf_event_gc_force(xf_event_t *const e)
{
    if (e == NULL) {
        return XF_FAIL;
    }
    e->ref_cnt = 0;
    return xf_event_gc(e);
}

xf_err_t xf_event_pool_set_gc_cb(xf_event_pool_id_t id,
                                 xf_event_gc_cb_t cb_func)
{
    if ((id == XF_EVENT_POOL_ID_STATIC)
            || (id >= XF_EVENT_GC_NUM_MAX)) {
        return XF_ERR_INVALID_ARG;
    }
    /* cppcheck-suppress misra-c2012-11.1 */
    s_gc[id] = cb_func;
    return XF_OK;
}

xf_event_gc_cb_t xf_event_pool_get_gc_cb(xf_event_pool_id_t id)
{
    if ((id == XF_EVENT_POOL_ID_STATIC)
            || (id >= XF_EVENT_GC_NUM_MAX)) {
        return NULL;
    }
    return s_gc[id];
}

/* ==================== [Static Functions] ================================== */

static xf_err_t xf_event_gc_cb_static(xf_event_t *const e)
{
    UNUSED(e);
    return XF_OK;
}

static xf_err_t xf_event_gc_cb_dyn(xf_event_t *const e)
{
    if (e == NULL) {
        return XF_FAIL;
    }
    /* TODO 动态事件 */
    // xf_free(e);
    return XF_ERR_NOT_SUPPORTED;
}
