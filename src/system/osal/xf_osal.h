/**
 * @file xf_osal.h
 * @author catcatBlue (catcatblue@qq.com)
 * @brief
 * @version 1.0
 * @date 2025-05-15
 *
 * SPDX-FileCopyrightText: 2025 CompanyNameMagicTag
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#ifndef __XF_OSAL_H__
#define __XF_OSAL_H__

/* ==================== [Includes] ========================================== */

#include "xf_common.h"

#if XF_ENABLE_OSAL

#include XF_OSAL_OS_TICK_HEADER_PATH
#include XF_OSAL_CMSIS_OS2_HEADER_PATH

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== [Defines] =========================================== */

/* ==================== [Typedefs] ========================================== */

/* ==================== [Global Prototypes] ================================= */

__STATIC_INLINE uint32_t osKernelTickToMs(uint32_t ticks)
{
    return osKernelGetTickFreq() * ticks / 1000;
}

__STATIC_INLINE uint32_t osKernelMsToTick(uint32_t ms)
{
    return osKernelGetTickFreq() * ms / 1000;
}

__STATIC_INLINE osStatus_t osDelayMs(uint32_t ms)
{
    return osDelay(osKernelMsToTick(ms));
}

/* ==================== [Macros] ============================================ */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* XF_ENABLE_OSAL */

#endif /* __XF_OSAL_H__ */
