# ------------------------------------------------------------------------------
# @brief cmake flie
# SPDX-FileCopyrightText: 2025 CompanyNameMagicTag
# SPDX-License-Identifier: Apache-2.0
# ------------------------------------------------------------------------------

if(CONFIG_XF_ENABLE_event)
    xf_component_append_if_exists(event)
endif()

if(CONFIG_XF_ENABLE_osal)
    xf_component_append_if_exists(osal)
endif()

if(CONFIG_XF_ENABLE_ps)
    xf_component_append_if_exists(ps)
endif()

if(CONFIG_XF_ENABLE_stimer)
    xf_component_append_if_exists(stimer)
endif()

if(CONFIG_XF_ENABLE_task)
    xf_component_append_if_exists(task)
endif()

if(CONFIG_XF_ENABLE_tick)
    xf_component_append_if_exists(tick)
endif()
