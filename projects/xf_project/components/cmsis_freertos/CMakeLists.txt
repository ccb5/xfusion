# ------------------------------------------------------------------------------
# @brief cmake flie
# SPDX-FileCopyrightText: 2025 CompanyNameMagicTag
# SPDX-License-Identifier: Apache-2.0
# ------------------------------------------------------------------------------

idf_component_register(
    SRCS
        "${CMAKE_CURRENT_LIST_DIR}/CMSIS/RTOS2/FreeRTOS/Source/cmsis_os2.c"
    INCLUDE_DIRS
        "${CMAKE_CURRENT_LIST_DIR}/CMSIS/RTOS2/FreeRTOS/Include"
    REQUIRES
        "freertos"
        "xfusion"
    WHOLE_ARCHIVE
)
