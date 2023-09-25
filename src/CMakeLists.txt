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

include_directories(
    ${GDAL_INCLUDE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/api
)

include(GNUInstallDirs)

macro(create_library_type target_name type)
    if(${type} STREQUAL "static")
        add_library(${target_name} STATIC
            ${SOURCES}
            ${HEADERS}
        )
    elseif(${type} STREQUAL "shared")
        add_library(${target_name} SHARED
            ${SOURCES}
            ${HEADERS}
        )
    else()
        message(FATAL_ERROR "Unknown library type `${type}`, CMake will exit.")
    endif()

    set_target_properties(${target_name} PROPERTIES OUTPUT_NAME ${LIBRARY_NAME})
    set_target_properties(${target_name} PROPERTIES VERSION ${PROJECT_VERSION})

    set_target_properties(${target_name} PROPERTIES PUBLIC_HEADER api/simplerasters.h)

    target_include_directories(${target_name} PRIVATE ${GDAL_INCLUDE_DIR})
    target_link_libraries(${target_name} PRIVATE ${GDAL_LIBRARY})

    install(TARGETS ${target_name}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${LIBRARY_NAME})
endmacro()

create_library_type(simplerasters_shared "shared")
create_library_type(simplerasters_static "static")
