# ------------------------------------------------------------------------------
# @brief cmake flie
# SPDX-FileCopyrightText: 2025 CompanyNameMagicTag
# SPDX-License-Identifier: Apache-2.0
# ------------------------------------------------------------------------------

if(CONFIG_XF_ENABLE_osal)
    xf_component_append_if_exists(osal)
endif()
