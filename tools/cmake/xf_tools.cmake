# ------------------------------------------------------------------------------
# @brief cmake flie
# SPDX-FileCopyrightText: 2025 CompanyNameMagicTag
# SPDX-License-Identifier: Apache-2.0
# ------------------------------------------------------------------------------

if(NOT XF_ROOT_DIR)
    message(FATAL_ERROR "XF_ROOT_DIR is not set. Please set it to the root directory of the XFusion.")
endif()

include("${XF_ROOT_DIR}/tools/cmake/utilities.cmake")
