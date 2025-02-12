# Copyright (c) 2021 Nordic Semiconductor ASA
# SPDX-License-Identifier: Apache-2.0
#
# This CMake file is picked by the Zephyr build system because it is defined
# as the module CMake entry point (see zephyr/module.yml).


# This is needed so that custom driver classes using system calls are taken into
# account
zephyr_syscall_include_directories(include)

zephyr_include_directories(include)

set(CMAKE_C_FLAGS "-O0 ${CMAKE_C_FLAGS}")
set(CMAKE_CXX_FLAGS "-O0 ${CMAKE_CXX_FLAGS}")

add_subdirectory(drivers)
add_subdirectory(lib)

zephyr_linker_sources(SECTIONS sections-rom.ld)
zephyr_linker_section(NAME test_http_service
                      KVMA RAM_REGION GROUP RODATA_REGION
                      SUBALIGN ${CONFIG_LINKER_ITERABLE_SUBALIGN})

set(gen_dir ${ZEPHYR_BINARY_DIR}/include/generated/)
set(source_file_index webpage/index.html)
generate_inc_file_for_target(app ${source_file_index} ${gen_dir}/index.html.gz.inc --gzip)