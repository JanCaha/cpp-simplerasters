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

    set_target_properties(${target_name} PROPERTIES OUTPUT_NAME ${LIBRARY_NAME_LOWER})
    set_target_properties(${target_name} PROPERTIES VERSION ${PROJECT_VERSION})

    set_target_properties(${target_name} PROPERTIES PUBLIC_HEADER api/simplerasters.h)

    target_include_directories(${target_name} PRIVATE ${GDAL_INCLUDE_DIR})
    target_include_directories(${target_name} PRIVATE ${CMAKE_SOURCE_DIR}"/src/api")

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
set_target_properties(simplerasters_static PROPERTIES
    POSITION_INDEPENDENT_CODE ON # Enable PIC for static lib
)

library_settings(simplerasters_shared)
library_settings(simplerasters_static)

include(GNUInstallDirs)

install(
    TARGETS simplerasters_shared simplerasters_static
    EXPORT SimpleRastersTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${LIBRARY_NAME}
)

install(
    EXPORT SimpleRastersTargets
    FILE SimpleRastersTargets.cmake
    NAMESPACE SimpleRasters::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${LIBRARY_NAME}
)
