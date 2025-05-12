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
        SRCS               # 对应模板中的 XF_SRCS
        # SRC_DIRS           # 源文件搜索目录
        # EXCLUDE_SRCS       # 排除源文件
        INCLUDE_DIRS       # 公共头文件目录（对应 XF_INCS_PUB）
        PRIV_INCLUDE_DIRS  # 私有头文件目录（对应 XF_INCS_PRIV）
        # LDFRAGMENTS        # 链接脚本片段
        REQUIRES           # 公共依赖（对应 XF_REQS_PUB）
        # PRIV_REQUIRES      # 私有依赖（对应 XF_REQS_PRIV）
        # EMBED_FILES        # 二进制嵌入文件
        # EMBED_TXTFILES     # 文本嵌入文件
    )
    cmake_parse_arguments(XF "${options}" "" "${multi_value}" ${ARGN})

    # 自动获取组件名（基于目录名）
    get_filename_component(COMPONENT_NAME ${CMAKE_CURRENT_LIST_DIR} NAME)
    
    # 路径规范化函数
    macro(normalize_paths output input base)
        set(${output} "")
        foreach(item ${input})
            get_filename_component(abs_item "${item}" ABSOLUTE BASE_DIR "${base}")
            list(APPEND ${output} "${abs_item}")
        endforeach()
    endmacro()

    # 处理源文件（支持 SRCS 或 SRC_DIRS+EXCLUDE_SRCS）
    set(final_sources "")
    # if(XF_SRCS)
        normalize_paths(final_sources "${XF_SRCS}" "${CMAKE_CURRENT_LIST_DIR}")
    # else()
    #     foreach(dir ${XF_SRC_DIRS})
    #         file(GLOB_RECURSE dir_sources
    #             "${CMAKE_CURRENT_LIST_DIR}/${dir}/*.c"
    #             "${CMAKE_CURRENT_LIST_DIR}/${dir}/*.cpp"
    #             "${CMAKE_CURRENT_LIST_DIR}/${dir}/*.S")
    #         list(APPEND final_sources ${dir_sources})
    #     endforeach()
    #     list(REMOVE_ITEM final_sources ${XF_EXCLUDE_SRCS})
    # endif()

    # 处理包含目录（转换为绝对路径）
    normalize_paths(abs_public_incs "${XF_INCLUDE_DIRS}" "${CMAKE_CURRENT_LIST_DIR}")
    normalize_paths(abs_private_incs "${XF_PRIV_INCLUDE_DIRS}" "${CMAKE_CURRENT_LIST_DIR}")

    # # 处理嵌入文件（转换为绝对路径）
    # normalize_paths(abs_embed_files "${XF_EMBED_FILES}" "${CMAKE_CURRENT_LIST_DIR}")
    # normalize_paths(abs_embed_txt "${XF_EMBED_TXTFILES}" "${CMAKE_CURRENT_LIST_DIR}")

    # 将数据持久化到全局属性
    set(prop_prefix "XF_COMPONENT_${COMPONENT_NAME}")
    set_property(GLOBAL PROPERTY ${prop_prefix}_SRCS          "${final_sources}")
    set_property(GLOBAL PROPERTY ${prop_prefix}_INCS_PUB      "${abs_public_incs}")
    set_property(GLOBAL PROPERTY ${prop_prefix}_INCS_PRIV     "${abs_private_incs}")
    set_property(GLOBAL PROPERTY ${prop_prefix}_REQS_PUB      "${XF_REQUIRES}")
    # set_property(GLOBAL PROPERTY ${prop_prefix}_REQS_PRIV     "${XF_PRIV_REQUIRES}")
    # set_property(GLOBAL PROPERTY ${prop_prefix}_LDFRAGMENTS   "${XF_LDFRAGMENTS}")
    # set_property(GLOBAL PROPERTY ${prop_prefix}_EMBED_FILES   "${abs_embed_files}")
    # set_property(GLOBAL PROPERTY ${prop_prefix}_EMBED_TXTFILES "${abs_embed_txt}")
    set_property(GLOBAL PROPERTY ${prop_prefix}_WHOLE_ARCHIVE "${XF_WHOLE_ARCHIVE}")

    # 记录注册状态
    set_property(GLOBAL APPEND PROPERTY XF_REGISTERED_COMPONENTS ${COMPONENT_NAME})
endfunction()

# @brief 组件汇总目标（应在顶层CMakeLists调用）
macro(xf_components_summary)
    # 定义汇总变量
    set(XF_SRCS_ALL "")
    set(XF_INCS_PUB_ALL "")
    set(XF_INCS_PRIV_ALL "")
    set(XF_REQS_PUB_ALL "")
    # set(XF_REQS_PRIV_ALL "")
    # set(XF_LDFRAGMENTS_ALL "")
    # set(XF_EMBED_FILES_ALL "")
    # set(XF_EMBED_TXTFILES_ALL "")
    # set(XF_WHOLE_ARCHIVE_LIST "")

    # 遍历所有已注册组件
    get_property(components GLOBAL PROPERTY XF_REGISTERED_COMPONENTS)
    foreach(comp ${components})
        set(prop_prefix "XF_COMPONENT_${comp}")

        # 源文件
        get_property(sources GLOBAL PROPERTY ${prop_prefix}_SRCS)
        list(APPEND XF_SRCS_ALL ${sources})

        # 包含目录
        get_property(incs_pub GLOBAL PROPERTY ${prop_prefix}_INCS_PUB)
        list(APPEND XF_INCS_PUB_ALL ${incs_pub})

        get_property(incs_priv GLOBAL PROPERTY ${prop_prefix}_INCS_PRIV)
        list(APPEND XF_INCS_PRIV_ALL ${incs_priv})

        # 依赖关系
        get_property(reqs_pub GLOBAL PROPERTY ${prop_prefix}_REQS_PUB)
        list(APPEND XF_REQS_PUB_ALL ${reqs_pub})

        # get_property(reqs_priv GLOBAL PROPERTY ${prop_prefix}_REQS_PRIV)
        # list(APPEND XF_REQS_PRIV_ALL ${reqs_priv})

        # # 链接脚本
        # get_property(ldfragments GLOBAL PROPERTY ${prop_prefix}_LDFRAGMENTS)
        # list(APPEND XF_LDFRAGMENTS_ALL ${ldfragments})

        # # 嵌入文件
        # get_property(embed_files GLOBAL PROPERTY ${prop_prefix}_EMBED_FILES)
        # list(APPEND XF_EMBED_FILES_ALL ${embed_files})

        # get_property(embed_txt GLOBAL PROPERTY ${prop_prefix}_EMBED_TXTFILES)
        # list(APPEND XF_EMBED_TXTFILES_ALL ${embed_txt})

        # # Whole-archive处理
        # get_property(whole_archive GLOBAL PROPERTY ${prop_prefix}_WHOLE_ARCHIVE)
        # if(whole_archive)
        #     list(APPEND XF_WHOLE_ARCHIVE_LIST ${comp})
        # endif()
    endforeach()

    # 全局去重
    foreach(var 
        XF_SRCS_ALL 
        XF_INCS_PUB_ALL 
        XF_INCS_PRIV_ALL 
        XF_REQS_PUB_ALL 
        # XF_REQS_PRIV_ALL 
        # XF_LDFRAGMENTS_ALL 
        # XF_EMBED_FILES_ALL 
        # XF_EMBED_TXTFILES_ALL
        )
        if(${var})
            list(REMOVE_DUPLICATES ${var})
        endif()
    endforeach()

    # 输出到缓存变量
    # set(XF_SRCS_ALL           "${XF_SRCS_ALL}"           CACHE INTERNAL "Aggregated source files")
    # set(XF_INCS_PUB_ALL       "${XF_INCS_PUB_ALL}"       CACHE INTERNAL "Public include directories")
    # set(XF_INCS_PRIV_ALL      "${XF_INCS_PRIV_ALL}"      CACHE INTERNAL "Private include directories")
    # set(XF_REQS_PUB_ALL       "${XF_REQS_PUB_ALL}"       CACHE INTERNAL "Public dependencies")
    # set(XF_REQS_PRIV_ALL      "${XF_REQS_PRIV_ALL}"      CACHE INTERNAL "Private dependencies")
    # set(XF_LDFRAGMENTS_ALL    "${XF_LDFRAGMENTS_ALL}"    CACHE INTERNAL "Linker script fragments")
    # set(XF_EMBED_FILES_ALL    "${XF_EMBED_FILES_ALL}"    CACHE INTERNAL "Embedded binary files")
    # set(XF_EMBED_TXTFILES_ALL "${XF_EMBED_TXTFILES_ALL}" CACHE INTERNAL "Embedded text files")
    # set(XF_WHOLE_ARCHIVE_LIST "${XF_WHOLE_ARCHIVE_LIST}" CACHE INTERNAL "Whole-archive components")
    
    set(XF_SRCS_ALL           "${XF_SRCS_ALL}"          )
    set(XF_INCS_PUB_ALL       "${XF_INCS_PUB_ALL}"      )
    set(XF_INCS_PRIV_ALL      "${XF_INCS_PRIV_ALL}"     )
    set(XF_REQS_PUB_ALL       "${XF_REQS_PUB_ALL}"      )
    # set(XF_REQS_PRIV_ALL      "${XF_REQS_PRIV_ALL}"     )
    # set(XF_LDFRAGMENTS_ALL    "${XF_LDFRAGMENTS_ALL}"   )
    # set(XF_EMBED_FILES_ALL    "${XF_EMBED_FILES_ALL}"   )
    # set(XF_EMBED_TXTFILES_ALL "${XF_EMBED_TXTFILES_ALL}")
    # set(XF_WHOLE_ARCHIVE_LIST "${XF_WHOLE_ARCHIVE_LIST}")
endmacro()