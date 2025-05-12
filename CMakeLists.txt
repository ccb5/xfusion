# ------------------------------------------------------------------------------
# @brief cmake flie
# SPDX-FileCopyrightText: 2025 CompanyNameMagicTag
# SPDX-License-Identifier: Apache-2.0
# ------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.12.4)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

if((NOT ESP_PLATFORM) OR (NOT XF_PLATFORM))
    if(NOT (CMAKE_C_COMPILER_ID STREQUAL "MSVC"))
        project(xfusion LANGUAGES C CXX ASM)
    else()
        project(xfusion LANGUAGES C CXX)
    endif()
endif()

set(XF_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR})
set(ENV{XF_ROOT_DIR} ${CMAKE_CURRENT_LIST_DIR})
set(XF_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}" CACHE STRING "XF_ROOT_DIR")

if(XF_PLATFORM)
    include(${XF_ROOT_DIR}/env_support/cmake/xf.cmake)
elseif(ESP_PLATFORM)
    include(${XF_ROOT_DIR}/env_support/cmake/esp.cmake)
else()
    include(${XF_ROOT_DIR}/env_support/cmake/custom.cmake)
endif()

#[[
    unfortunately CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS does not work for global data. 
    for global data we still need decl specs.
    Check out the docs to learn more about the limitations of CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS
    https://cmake.org/cmake/help/latest/prop_tgt/WINDOWS_EXPORT_ALL_SYMBOLS.html#prop_tgt:WINDOWS_EXPORT_ALL_SYMBOLS

    For all compiled sources within the library (i.e. basically all xfusion files) we need to use dllexport.
    For all compiled sources from outside the library (i.e. files which include xfusion headers) we need to use dllimport.
    We can do this by using CMakes INTERFACE and PRIVATE keyword.
  ]]
if (MSVC)
    target_compile_definitions(
        xfusion
        INTERFACE
            XF_ATTR_EXTERN_DATA=__declspec\(dllimport\)
        PRIVATE
            XF_ATTR_EXTERN_DATA=__declspec\(dllexport\)
    )
endif()
