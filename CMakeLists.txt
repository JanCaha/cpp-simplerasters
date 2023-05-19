cmake_minimum_required(VERSION 3.20)

project(simplerasters VERSION 0.2.0 DESCRIPTION "Raster Representation Library")

option(BUILD_TESTS "Build tests." ON)

set(LIBRARY_NAME "simplerasters")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

find_package(GDAL REQUIRED)

add_subdirectory(src)

if(BUILD_TESTS)
    include(CTest)
    enable_testing(true)

    add_subdirectory(tests)
endif()

configure_file(${LIBRARY_NAME}.pc.in ${LIBRARY_NAME}.pc @ONLY)

install(FILES ${CMAKE_BINARY_DIR}/${LIBRARY_NAME}.pc DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)