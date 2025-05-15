# ------------------------------------------------------------------------------
# @brief cmake flie
# SPDX-FileCopyrightText: 2025 CompanyNameMagicTag
# SPDX-License-Identifier: Apache-2.0
# ------------------------------------------------------------------------------

idf_component_register(
    SRCS 
        "main.c"
        "app_main.c"
        "xf_main.c"
    INCLUDE_DIRS 
        "."
    # REQUIRES
    WHOLE_ARCHIVE
)
