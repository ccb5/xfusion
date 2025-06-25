/**
 * @file main.c
 * @author catcatBlue (catcatblue@qq.com)
 * @brief
 * @version 1.0
 * @date 2025-05-15
 *
 * SPDX-FileCopyrightText: 2025 CompanyNameMagicTag
 * SPDX-License-Identifier: Apache-2.0
 *
 */

/* ==================== [Includes] ========================================== */

#include "xf_osal.h"

/* ==================== [Defines] =========================================== */

/* ==================== [Typedefs] ========================================== */

/* ==================== [Static Prototypes] ================================= */

extern void app_main(void *arg);

/* ==================== [Static Variables] ================================== */

/* ==================== [Macros] ============================================ */

/* ==================== [Global Functions] ================================== */

int main(void)
{
    osKernelInitialize();

    osThreadAttr_t attr = {
        .name       = "app_main",
        .stack_size = 8 * 1024,
        .priority   = osPriorityNormal,
        .cb_mem     = NULL,
        .cb_size    = 0,
    };

    osKernelLock();
    osThreadNew(app_main, NULL, &attr);
    osKernelUnlock();

    osKernelStart();

    return 0;
}

/* ==================== [Static Functions] ================================== */
