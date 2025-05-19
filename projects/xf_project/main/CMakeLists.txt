# ------------------------------------------------------------------------------
# @brief cmake flie
# SPDX-FileCopyrightText: 2025 CompanyNameMagicTag
# SPDX-License-Identifier: Apache-2.0
# ------------------------------------------------------------------------------

file(GLOB SRC_LIST ${CMAKE_CURRENT_LIST_DIR}/test_co/*.c)

idf_component_register(
    SRCS 
        "main.c"
        "app_main.c"
        "xf_main.c"
        "${SRC_LIST}"
    INCLUDE_DIRS 
        "."
        "test_co"
    # REQUIRES
    WHOLE_ARCHIVE
)
