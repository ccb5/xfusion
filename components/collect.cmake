# ------------------------------------------------------------------------------
# @brief cmake flie
# SPDX-FileCopyrightText: 2025 CompanyNameMagicTag
# SPDX-License-Identifier: Apache-2.0
# ------------------------------------------------------------------------------

# if(CONFIG_XF_ENABLE_algo)
#     xf_component_append_if_exists(algo)
# endif()

if(CONFIG_XF_ENABLE_common)
    xf_component_append_if_exists(common)
endif()

if(CONFIG_XF_ENABLE_dstruct)
    xf_component_append_if_exists(dstruct)
endif()

if(CONFIG_XF_ENABLE_log)
    xf_component_append_if_exists(log)
endif()

if(CONFIG_XF_ENABLE_system)
    xf_collect_if_exists(system)
endif()
