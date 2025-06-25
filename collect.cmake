# ------------------------------------------------------------------------------
# @brief cmake flie
# SPDX-FileCopyrightText: 2025 CompanyNameMagicTag
# SPDX-License-Identifier: Apache-2.0
# ------------------------------------------------------------------------------

# collect.cmake 要求构建系统支持将 Kconfig 配置转为 CMake 变量
# 以实现通过 Kconfig 配置编译选项，如配置某些模块是否参与编译。

xf_collect_if_exists(components)

if(CONFIG_XF_ENABLE_EXAMPLES)
    xf_collect_if_exists(examples)
endif()
