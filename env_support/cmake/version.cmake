# ------------------------------------------------------------------------------
# @brief cmake flie
# SPDX-FileCopyrightText: 2025 CompanyNameMagicTag
# SPDX-License-Identifier: Apache-2.0
# ------------------------------------------------------------------------------

set(XF_VERSION_MAJOR "2")
set(XF_VERSION_MINOR "0")
set(XF_VERSION_PATCH "0")
set(XF_VERSION_INFO  "")

set(XF_VERSION ${XF_VERSION_MAJOR}.${XF_VERSION_MINOR}.${XF_VERSION_PATCH}) # This is a CMake variable
set(ENV{XF_VERSION} ${XF_VERSION}) # This is exported Environmental variable
set(XF_SOVERSION ${XF_VERSION_MAJOR})
