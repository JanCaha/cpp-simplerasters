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
    set_target_properties(${target_name} PROPERTIES OUTPUT_NAME ${LIBRARY_NAME})
    set_target_properties(${target_name} PROPERTIES VERSION ${PROJECT_VERSION})

    set_target_properties(${target_name} PROPERTIES PUBLIC_HEADER api/simplerasters.h)

    target_include_directories(${target_name} PRIVATE ${GDAL_INCLUDE_DIR})
    target_include_directories(${target_name} PUBLIC ${CMAKE_SOURCE_DIR}"/src/api")

    target_link_libraries(${target_name} PRIVATE ${GDAL_LIBRARY})

    # include(GNUInstallDirs)
    install(TARGETS ${target_name}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${LIBRARY_NAME})
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
