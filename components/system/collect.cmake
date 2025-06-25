# ------------------------------------------------------------------------------
# @brief cmake flie
# SPDX-FileCopyrightText: 2025 CompanyNameMagicTag
# SPDX-License-Identifier: Apache-2.0
# ------------------------------------------------------------------------------

if(CONFIG_XF_ENABLE_EVENT)
    xf_component_append_if_exists(event)
endif()

if(CONFIG_XF_ENABLE_OSAL)
    xf_component_append_if_exists(osal)
endif()

if(CONFIG_XF_ENABLE_PS)
    xf_component_append_if_exists(ps)
endif()

if(CONFIG_XF_ENABLE_SAFE)
    xf_component_append_if_exists(safe)
endif()

if(CONFIG_XF_ENABLE_STIMER)
    xf_component_append_if_exists(stimer)
endif()

if(CONFIG_XF_ENABLE_TASK)
    xf_component_append_if_exists(task)
endif()

if(CONFIG_XF_ENABLE_TICK)
    xf_component_append_if_exists(tick)
endif()
