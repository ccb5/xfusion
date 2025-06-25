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

#include "cmsis_rtos2/os_tick.h"
#include "cmsis_rtos2/cmsis_os2.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== [Defines] =========================================== */

/* ==================== [Typedefs] ========================================== */

/* ==================== [Global Prototypes] ================================= */

static inline uint32_t osKernelTickToMs(uint32_t ticks)
{
    return osKernelGetTickFreq() * ticks / 1000;
}

static inline uint32_t osKernelMsToTick(uint32_t ms)
{
    return osKernelGetTickFreq() * ms / 1000;
}

static inline osStatus_t osDelayMs(uint32_t ms)
{
    return osDelay(osKernelMsToTick(ms));
}

/* ==================== [Macros] ============================================ */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __XF_OSAL_H__ */
