cmake_minimum_required(VERSION 3.23)

project(simplerasters VERSION 0.3.4 DESCRIPTION "Raster Representation Library")

option(BUILD_TESTS "Build tests." ON)
option(PACK_DEB "Build tests." ON)

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

# ############################################################
# Pack :
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    if(PACK_DEB)
        message(STATUS "DEB will be packed file.")

        include(Packing)

        file(GLOB old_versions "${CMAKE_SOURCE_DIR}/_packages/*.deb")

        if(old_versions)
            file(REMOVE ${old_versions})
        endif()

        add_custom_target(pack_deb
            COMMAND ${CMAKE_CPACK_COMMAND} -G DEB -C Release
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        )
    endif()
endif()