# ------------------------------------------------------------------------------
# @brief cmake flie
# SPDX-FileCopyrightText: 2025 CompanyNameMagicTag
# SPDX-License-Identifier: Apache-2.0
# ------------------------------------------------------------------------------

include("${CMAKE_CURRENT_LIST_DIR}/version.cmake")

# 初始化以及包含必要脚本
include("${XF_ROOT_DIR}/tools/cmake/xf_tools.cmake")

set(EXCLUDE_PATHS 
    ".backup"
    ".temp"
    ".dummy"
)

file(GLOB_RECURSE ALL_SOURCES 
    ${XF_ROOT_DIR}/src/*.c 
    ${XF_ROOT_DIR}/src/*.cpp
)

set(XF_SRCS "")
foreach(src ${ALL_SOURCES})
    set(SHOULD_EXCLUDE FALSE)
    foreach(exclude_path ${EXCLUDE_PATHS})
        if(src MATCHES "/${exclude_path}/")
            set(SHOULD_EXCLUDE TRUE)
            break()
        endif()
    endforeach()
    if(NOT SHOULD_EXCLUDE)
        list(APPEND XF_SRCS ${src})
    endif()
endforeach()

idf_component_register(
    SRCS 
        "${XF_SRCS}"
    INCLUDE_DIRS 
        "${XF_ROOT_DIR}"
        "${XF_ROOT_DIR}/../"
        "${XF_ROOT_DIR}/examples"
    # REQUIRES # TODO
    WHOLE_ARCHIVE
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
