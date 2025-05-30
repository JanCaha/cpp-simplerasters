cmake_minimum_required(VERSION 3.23)

project(simplerasters VERSION 0.8.1 DESCRIPTION "C++ Raster Data Access Library based on GDAL")

option(BUILD_TESTS "Build tests." ON)
option(PACK_DEB "Build tests." ON)

set(LIBRARY_NAME "SimpleRasters")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

find_package(GDAL REQUIRED)

add_subdirectory(src)

# Create and install package configuration files
include(CMakePackageConfigHelpers)

# Configure version file
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_NAME}ConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

# Configure config file template
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_NAME}Config.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${LIBRARY_NAME}
)

# Install configuration files
install(
    FILES
    "${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_NAME}Config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_NAME}ConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${LIBRARY_NAME}
)

if(BUILD_TESTS)
    include(CTest)
    enable_testing(true)

    add_subdirectory(tests)
endif()

string(FIND ${CMAKE_INSTALL_PREFIX} "debian/tmp/usr" IS_PPA_BUILD)

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