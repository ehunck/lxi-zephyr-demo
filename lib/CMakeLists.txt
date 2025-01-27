# Add SCPI library source files
file(GLOB SCPI_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/scpi/libscpi/src/*.c)

# Include the SCPI library headers
zephyr_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/scpi/libscpi/inc)
zephyr_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/scpi/libscpi/src)

# Add SCPI sources to the build
target_sources(app PRIVATE ${SCPI_SOURCES})
