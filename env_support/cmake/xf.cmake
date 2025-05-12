# ------------------------------------------------------------------------------
# @brief cmake flie
# SPDX-FileCopyrightText: 2025 CompanyNameMagicTag
# SPDX-License-Identifier: Apache-2.0
# ------------------------------------------------------------------------------

include("${CMAKE_CURRENT_LIST_DIR}/version.cmake")

# 初始化以及包含必要脚本
include("${XF_ROOT_DIR}/tools/cmake/xf_tools.cmake")

# 收集/枚举所有组件
xf_collect_if_exists("${XF_ROOT_DIR}")

message("XF_ROOT_DIR: ${XF_ROOT_DIR}")
message("XF_COMPONENTS: ${XF_COMPONENTS}")

foreach(xf_component ${XF_COMPONENTS})
    add_subdirectory(${xf_component})
endforeach()

# 汇总所有 xf 组件的信息，当前做法是多个 xf 组件变一个 idf 组件
xf_components_summary()

message("XF_SRCS_ALL: ${XF_SRCS_ALL}")
message("XF_INCS_PUB_ALL: ${XF_INCS_PUB_ALL}")
message("XF_INCS_PRIV_ALL: ${XF_INCS_PRIV_ALL}")
message("XF_REQS_PUB_ALL: ${XF_REQS_PUB_ALL}")

idf_component_register(
    SRCS 
        ${XF_SRCS_ALL} 
    INCLUDE_DIRS 
        ${XF_ROOT_DIR}
        ${XF_ROOT_DIR}/../
        ${XF_INCS_PUB_ALL}
)

# # TODOTODOTODO 没改完

# target_compile_definitions(${COMPONENT_LIB} PUBLIC "-DLV_CONF_INCLUDE_SIMPLE")

# if(CONFIG_XF_ATTR_FAST_MEM_USE_IRAM)
#     target_compile_definitions(${COMPONENT_LIB} PUBLIC "-DXF_ATTR_FAST_MEM=IRAM_ATTR")
# endif()

# if(CONFIG_FREERTOS_SMP)
#     target_include_directories(${COMPONENT_LIB} PRIVATE "${IDF_PATH}/components/freertos/FreeRTOS-Kernel-SMP/include/freertos/")
# else()
#     target_include_directories(${COMPONENT_LIB} PRIVATE "${IDF_PATH}/components/freertos/FreeRTOS-Kernel/include/freertos/")
# endif()
