# ------------------------------------------------------------------------------
# @brief cmake flie
# @remarks components_template version v1.0
# SPDX-FileCopyrightText: 2025 CompanyNameMagicTag
# SPDX-License-Identifier: Apache-2.0
# ------------------------------------------------------------------------------

# 初始化以及包含必要脚本
include("$ENV{XF_ROOT_DIR}/tools/cmake/xf_tools.cmake")

xf_component_register(
    SRCS                    # 源文件列表
        "${CMAKE_CURRENT_LIST_DIR}/xf_log.c"
    INCS_PUB                # 头文件目录列表
        "${CMAKE_CURRENT_LIST_DIR}"
        "${CMAKE_CURRENT_LIST_DIR}/include"
    # DEFS_PRIV               # （不推荐使用）私有全局宏定义
    # COMPILE_OPTS_PRIV       # （不推荐使用）私有编译选项
    # LINK_OPTS_PRIV          # （不推荐使用）私有链接选项
    # LIBS                    # （不推荐使用）静态库列表
    # REQS_PUB                # 公共依赖
    WHOLE_ARCHIVE           # 强制链接静态库中的所有目标
)
