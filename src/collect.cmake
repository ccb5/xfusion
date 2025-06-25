# ------------------------------------------------------------------------------
# @brief cmake flie
# SPDX-FileCopyrightText: 2025 CompanyNameMagicTag
# SPDX-License-Identifier: Apache-2.0
# ------------------------------------------------------------------------------

if(CONFIG_XF_ENABLE_ALGO)
    xf_component_append_if_exists(algo)
endif()

if(CONFIG_XF_ENABLE_COMMON)
    xf_component_append_if_exists(common)
endif()

if(CONFIG_XF_ENABLE_DSTRUCT)
    xf_component_append_if_exists(dstruct)
endif()

if(CONFIG_XF_ENABLE_LOG)
    xf_component_append_if_exists(log)
endif()

if(CONFIG_XF_ENABLE_STD)
    xf_component_append_if_exists(std)
endif()

if(CONFIG_XF_ENABLE_SYSTEM)
    xf_collect_if_exists(system)
endif()

if(CONFIG_XF_ENABLE_UTILS)
    xf_component_append_if_exists(utils)
endif()
