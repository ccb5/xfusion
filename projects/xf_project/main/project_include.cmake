# ------------------------------------------------------------------------------
# @brief cmake flie
# SPDX-FileCopyrightText: 2025 CompanyNameMagicTag
# SPDX-License-Identifier: Apache-2.0
# ------------------------------------------------------------------------------

set(TOOLCHAIN_PREFIX "")
# set(CMAKE_C_COMPILER "${TOOLCHAIN_PREFIX}gcc")
# set(CMAKE_ASM_COMPILER "${TOOLCHAIN_PREFIX}gcc")
# set(CMAKE_CXX_COMPILER "${TOOLCHAIN_PREFIX}g++")
# set(CMAKE_AR "${TOOLCHAIN_PREFIX}ar")
# set(CMAKE_LINKER "{TOOLCHAIN_PREFIX}ld")
# set(CMAKE_OBJCOPY "${TOOLCHAIN_PREFIX}objcopy")
set(CMAKE_OBJDUMP "${TOOLCHAIN_PREFIX}objdump")
set(CMAKE_READELF "${TOOLCHAIN_PREFIX}readelf")
# set(CMAKE_RANLIB "${TOOLCHAIN_PREFIX}ranlib")
set(CMAKE_SIZE "${TOOLCHAIN_PREFIX}size")
set(CMAKE_MN "${TOOLCHAIN_PREFIX}nm")
# set(CMAKE_STRIP "${TOOLCHAIN_PREFIX}ld")

idf_build_get_property(build_dir BUILD_DIR)

idf_build_get_property(elf_name EXECUTABLE_NAME GENERATOR_EXPRESSION)
idf_build_get_property(elf EXECUTABLE GENERATOR_EXPRESSION)

set(PROJECT_READELF "${elf_name}.txt")
set(PROJECT_NM "${elf_name}.nm")
set(PROJECT_OBJDUMP "${elf_name}.lst")

add_custom_command(
    OUTPUT "${build_dir}/bin_tmp"
    COMMAND "${CMAKE_READELF}" -a -h -l -S -g -s "${build_dir}/${elf}" >
            "${build_dir}/${PROJECT_READELF}"
    COMMAND "${CMAKE_MN}" -n -l -C -a -A -g "${build_dir}/${elf}" >
            "${build_dir}/${PROJECT_NM}"
    COMMAND "${CMAKE_OBJDUMP}" -d "${build_dir}/${elf}" >
            "${build_dir}/${PROJECT_OBJDUMP}"
    COMMAND "${CMAKE_SIZE}" "${build_dir}/${elf}"
    DEPENDS ${elf}
    VERBATIM
    WORKING_DIRECTORY ${build_dir}
    COMMENT "Generating binary image from built executable"
)

add_custom_target(gen_project_binary DEPENDS "${build_dir}/bin_tmp")
add_custom_target(app ALL DEPENDS gen_project_binary)

add_custom_target(
    run
    COMMAND "${build_dir}/${elf}"
    DEPENDS ${elf}
)
