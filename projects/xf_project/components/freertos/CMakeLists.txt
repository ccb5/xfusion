# ------------------------------------------------------------------------------
# @brief cmake flie
# SPDX-FileCopyrightText: 2025 CompanyNameMagicTag
# SPDX-License-Identifier: Apache-2.0
# ------------------------------------------------------------------------------

# Set the correct FreeRTOS port for your system (e.g., Posix for WSL)
set(FREERTOS_PORT GCC_POSIX CACHE STRING "Port for FreeRTOS on Posix environment")

file(GLOB FREERTOS_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/FreeRTOS-Kernel/*.c"
    "${CMAKE_CURRENT_LIST_DIR}/FreeRTOS-Kernel/portable/MemMang/heap_4.c"
    "${CMAKE_CURRENT_LIST_DIR}/FreeRTOS-Kernel/portable/ThirdParty/GCC/Posix/*.c"
)

idf_component_register(
    SRCS
        "${CMAKE_CURRENT_LIST_DIR}/FreeRTOS_Posix_Port.c"
        "${FREERTOS_SOURCES}"
    INCLUDE_DIRS
        "${CMAKE_CURRENT_LIST_DIR}/FreeRTOS-Kernel/include"
        "${CMAKE_CURRENT_LIST_DIR}/FreeRTOS-Kernel/portable/ThirdParty/GCC/Posix"
        "${CMAKE_CURRENT_LIST_DIR}/config"
    # REQUIRES
    WHOLE_ARCHIVE
)
