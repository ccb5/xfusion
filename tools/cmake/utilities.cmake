# ------------------------------------------------------------------------------
# @brief cmake flie
# SPDX-FileCopyrightText: 2025 CompanyNameMagicTag
# SPDX-License-Identifier: Apache-2.0
# ------------------------------------------------------------------------------

if(NOT COMMAND include_if_exists)
    macro(include_if_exists path)
        if(EXISTS "${path}")
            include("${path}")
        endif()
    endmacro()
endif()

if(NOT COMMAND include_if_exists_relative)
    macro(include_if_exists_relative path)
        if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/${path}")
            include("${CMAKE_CURRENT_LIST_DIR}/${path}")
        endif()
    endmacro()
endif()

# @brief 自动识别绝对/相对路径的智能包含宏
# @param path 待包含的路径（支持绝对路径或基于当前文件的相对路径）
macro(xf_collect_if_exists dir_path)
    get_filename_component(abs_dir "${dir_path}" ABSOLUTE
        BASE_DIR "${CMAKE_CURRENT_LIST_DIR}")
    get_filename_component(norm_dir "${abs_dir}" REALPATH)
    set(collect_file "${norm_dir}/collect.cmake")
    if(EXISTS "${collect_file}")
        include("${collect_file}")
    else()
        message(STATUS "Skip missing component: ${collect_file}")
    endif()
endmacro()

# 路径规范化函数
macro(normalize_paths output input base)
    set(${output} "")
    foreach(item ${input})
        get_filename_component(abs_item "${item}" ABSOLUTE BASE_DIR "${base}")
        list(APPEND ${output} "${abs_item}")
    endforeach()
endmacro()

# @brief 如果该路径存在，则附加到 xf 组件列表(EXTRA_COMPONENT_DIRS)中。
macro(xf_component_append_if_exists dir_path)
    get_filename_component(abs_dir "${dir_path}" ABSOLUTE
        BASE_DIR "${CMAKE_CURRENT_LIST_DIR}")
    get_filename_component(norm_dir "${abs_dir}" REALPATH)
        message(STATUS "Append component: ${norm_dir}")
    if(EXISTS "${norm_dir}")
        list(APPEND XF_COMPONENTS "${norm_dir}")
        list(REMOVE_DUPLICATES XF_COMPONENTS)
    endif()
endmacro()

# @brief 组件注册函数
function(xf_component_register)
    # 参数解析
    set(options WHOLE_ARCHIVE)
    set(multi_value 
        SRCS                # 源文件列表
        INCS_PUB            # 头文件目录列表
        DEFS_PRIV           # （不推荐使用）私有全局宏定义
        COMPILE_OPTS_PRIV   # （不推荐使用）私有编译选项
        LINK_OPTS_PRIV      # （不推荐使用）私有链接选项
        LIBS                # （不推荐使用）静态库列表
        REQS_PUB            # 公共依赖
    )
    cmake_parse_arguments(_ "${options}" "" "${multi_value}" ${ARGN})

    # 自动获取组件名（基于目录名）
    get_filename_component(COMPONENT_NAME ${CMAKE_CURRENT_LIST_DIR} NAME)
    
    normalize_paths(__SRCS              "${__SRCS}"                 "${CMAKE_CURRENT_LIST_DIR}")
    normalize_paths(__INCS_PUB          "${__INCS_PUB}"             "${CMAKE_CURRENT_LIST_DIR}")
    normalize_paths(__DEFS_PRIV         "${__DEFS_PRIV}"            "${CMAKE_CURRENT_LIST_DIR}")
    normalize_paths(__COMPILE_OPTS_PRIV "${__COMPILE_OPTS_PRIV}"    "${CMAKE_CURRENT_LIST_DIR}")
    normalize_paths(__LINK_OPTS_PRIV    "${__LINK_OPTS_PRIV}"       "${CMAKE_CURRENT_LIST_DIR}")
    normalize_paths(__LIBS              "${__LIBS}"                 "${CMAKE_CURRENT_LIST_DIR}")
    normalize_paths(__REQS_PUB          "${__REQS_PUB}"             "${CMAKE_CURRENT_LIST_DIR}")

    # 将数据持久化到全局属性
    set(prop_prefix "XF_COMPONENT_${COMPONENT_NAME}")
    set_property(GLOBAL PROPERTY ${prop_prefix}_PATH                "${CMAKE_CURRENT_LIST_DIR}")
    set_property(GLOBAL PROPERTY ${prop_prefix}_SRCS                "${__SRCS}")
    set_property(GLOBAL PROPERTY ${prop_prefix}_INCS_PUB            "${__INCS_PUB}")
    set_property(GLOBAL PROPERTY ${prop_prefix}_DEFS_PRIV           "${__DEFS_PRIV}")
    set_property(GLOBAL PROPERTY ${prop_prefix}_COMPILE_OPTS_PRIV   "${__COMPILE_OPTS_PRIV}")
    set_property(GLOBAL PROPERTY ${prop_prefix}_LINK_OPTS_PRIV      "${__LINK_OPTS_PRIV}")
    set_property(GLOBAL PROPERTY ${prop_prefix}_LIBS                "${__LIBS}")
    set_property(GLOBAL PROPERTY ${prop_prefix}_REQS_PUB            "${__REQS_PUB}")
    set_property(GLOBAL PROPERTY ${prop_prefix}_WHOLE_ARCHIVE       "${__WHOLE_ARCHIVE}")

    # 记录注册状态
    set_property(GLOBAL APPEND PROPERTY XF_REGISTERED_COMPONENTS ${COMPONENT_NAME})
endfunction()

# @brief 组件汇总目标
function(xf_components_summary)
    # 定义汇总变量
    set(XF_COMPONENTS_PATH_ALL "")
    set(XF_SRCS_ALL "")
    set(XF_INCS_PUB_ALL "")
    set(XF_DEFS_PRIV_ALL "")
    set(XF_COMPILE_OPTS_PRIV_ALL "")
    set(XF_LINK_OPTS_PRIV_ALL "")
    set(XF_LIBS_ALL "")
    set(XF_REQS_PUB_ALL "")
    # set(XF_WHOLE_ARCHIVE_LIST "") # 暂不需要

    # 遍历所有已注册组件
    get_property(components GLOBAL PROPERTY XF_REGISTERED_COMPONENTS)
    foreach(comp ${components})
        set(prop_prefix "XF_COMPONENT_${comp}")
        get_property(path GLOBAL PROPERTY ${prop_prefix}_PATH)
        list(APPEND XF_COMPONENTS_PATH_ALL ${path})
        get_property(sources GLOBAL PROPERTY ${prop_prefix}_SRCS)
        list(APPEND XF_SRCS_ALL ${sources})
        get_property(incs_pub GLOBAL PROPERTY ${prop_prefix}_INCS_PUB)
        list(APPEND XF_INCS_PUB_ALL ${incs_pub})
        get_property(defs_priv GLOBAL PROPERTY ${prop_prefix}_DEFS_PRIV)
        list(APPEND XF_DEFS_PRIV_ALL ${defs_priv})
        get_property(compile_opts_priv GLOBAL PROPERTY ${prop_prefix}_COMPILE_OPTS_PRIV)
        list(APPEND XF_COMPILE_OPTS_PRIV_ALL ${compile_opts_priv})
        get_property(link_opts_priv GLOBAL PROPERTY ${prop_prefix}_LINK_OPTS_PRIV)
        list(APPEND XF_LINK_OPTS_PRIV_ALL ${link_opts_priv})
        get_property(libs GLOBAL PROPERTY ${prop_prefix}_LIBS)
        list(APPEND XF_LIBS_ALL ${libs})
        get_property(reqs_pub GLOBAL PROPERTY ${prop_prefix}_REQS_PUB)
        list(APPEND XF_REQS_PUB_ALL ${reqs_pub})
        # get_property(whole_archive GLOBAL PROPERTY ${prop_prefix}_WHOLE_ARCHIVE)
        # if(whole_archive)
        #     list(APPEND XF_WHOLE_ARCHIVE_LIST ${comp})
        # endif()
    endforeach()

    # 全局去重
    foreach(var 
        XF_SRCS_ALL
        XF_INCS_PUB_ALL
        XF_DEFS_PRIV_ALL
        XF_COMPILE_OPTS_PRIV_ALL
        XF_LINK_OPTS_PRIV_ALL
        XF_LIBS_ALL
        XF_REQS_PUB_ALL
        # XF_WHOLE_ARCHIVE_LIST
        )
        if(${var})
            list(REMOVE_DUPLICATES ${var})
        endif()
    endforeach()

    # 输出汇总结果
    set(XF_COMPONENTS_PATH_ALL      "${XF_COMPONENTS_PATH_ALL}" PARENT_SCOPE)
    set(XF_SRCS_ALL                 "${XF_SRCS_ALL}" PARENT_SCOPE)
    set(XF_INCS_PUB_ALL             "${XF_INCS_PUB_ALL}" PARENT_SCOPE)
    set(XF_DEFS_PRIV_ALL            "${XF_DEFS_PRIV_ALL}" PARENT_SCOPE)
    set(XF_COMPILE_OPTS_PRIV_ALL    "${XF_COMPILE_OPTS_PRIV_ALL}" PARENT_SCOPE)
    set(XF_LINK_OPTS_PRIV_ALL       "${XF_LINK_OPTS_PRIV_ALL}" PARENT_SCOPE)
    set(XF_LIBS_ALL                 "${XF_LIBS_ALL}" PARENT_SCOPE)
    set(XF_REQS_PUB_ALL             "${XF_REQS_PUB_ALL}" PARENT_SCOPE)
    # set(XF_WHOLE_ARCHIVE_LIST "${XF_WHOLE_ARCHIVE_LIST}" PARENT_SCOPE)
endfunction()