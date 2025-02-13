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
zephyr_linker_section(NAME 
                    http_resource_desc_test_http_service
                    KVMA RAM_REGION GROUP RODATA_REGION
                    SUBALIGN ${CONFIG_LINKER_ITERABLE_SUBALIGN})

set(gen_dir ${ZEPHYR_BINARY_DIR}/include/generated/)

foreach(web_resource
  index.html
  millipede-logo-dark-gray.png
    )
  generate_inc_file_for_target(
    app
    webpage/${web_resource}
    ${gen_dir}/${web_resource}.gz.inc
    --gzip
  )
endforeach()

