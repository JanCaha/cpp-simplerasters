SET(SOURCES
    abstractraster.cpp
    singlebandraster.cpp
    projectedsquarecellraster.cpp
    helpers.cpp
)

SET(HEADERS
    api/simplerasters.h
    helpers.h
)

macro(library_settings target_name)
    string(TOLOWER ${LIBRARY_NAME} LIBRARY_NAME_LOWER)

    get_target_property(target_type ${target_name} TYPE)

    if (WIN32)
        if(target_type STREQUAL "STATIC_LIBRARY")
            set_target_properties(${target_name} PROPERTIES OUTPUT_NAME "${LIBRARY_NAME_LOWER}_static")
        else()
            set_target_properties(${target_name} PROPERTIES OUTPUT_NAME ${LIBRARY_NAME_LOWER})
            target_compile_definitions(${target_name} PRIVATE SIMPLERASTERS_EXPORTS)
        endif()
    else()
        set_target_properties(${target_name} PROPERTIES OUTPUT_NAME ${LIBRARY_NAME_LOWER})
    endif()

    set_target_properties(${target_name} PROPERTIES VERSION ${PROJECT_VERSION})

    include(GNUInstallDirs)
  
    target_include_directories(${target_name}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/api>
        $<INSTALL_INTERFACE:include/${LIBRARY_NAME}>
    PRIVATE
        ${GDAL_INCLUDE_DIR}
    )

    target_link_libraries(${target_name} PRIVATE ${GDAL_LIBRARY})
endmacro(library_settings)

include_directories(
    ${GDAL_INCLUDE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/api
)

add_library(simplerasters_shared SHARED
    ${SOURCES}
)

add_library(simplerasters_static STATIC
    ${SOURCES}
)

library_settings(simplerasters_shared)
library_settings(simplerasters_static)

set_target_properties(simplerasters_static PROPERTIES
    POSITION_INDEPENDENT_CODE ON # Enable PIC for static lib
)
    
include(GNUInstallDirs)

install(
    TARGETS simplerasters_static simplerasters_shared
    EXPORT SimpleRastersTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${LIBRARY_NAME}
)

install(
    DIRECTORY ${CMAKE_SOURCE_DIR}/src/api/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${LIBRARY_NAME}
    FILES_MATCHING PATTERN "*.h"
)

install(
    EXPORT SimpleRastersTargets
    FILE SimpleRastersTargets.cmake
    NAMESPACE SimpleRasters::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${LIBRARY_NAME}
)
