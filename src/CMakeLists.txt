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

add_library(simplerasters SHARED
    ${SOURCES}
    ${HEADERS}
)

set_target_properties(simplerasters PROPERTIES OUTPUT_NAME ${LIBRARY_NAME})
set_target_properties(simplerasters PROPERTIES VERSION ${PROJECT_VERSION})

set_target_properties(simplerasters PROPERTIES PUBLIC_HEADER api/simplerasters.h)

target_include_directories(simplerasters PRIVATE ${GDAL_INCLUDE_DIR})
target_link_libraries(simplerasters PRIVATE ${GDAL_LIBRARY})

include(GNUInstallDirs)

install(TARGETS simplerasters
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${LIBRARY_NAME})

# Adjust install paths for Conda on Windows
if(WIN32)
    message(STATUS "Windows detected.")

    set(CONDA_PREFIX $ENV{CONDA_PREFIX})

    if(CONDA_PREFIX)
        message(STATUS "CONDA PREFIX detected adjusting install paths.")

        install(TARGETS simplerasters
            LIBRARY DESTINATION ${CONDA_PREFIX}/Library/lib
            PUBLIC_HEADER DESTINATION ${CONDA_PREFIX}/Library/include/${LIBRARY_NAME})
    else()
        message(FATAL_ERROR "Conda environment not detected. Please activate a Conda environment.")
    endif()

# If not Windows, use the default install paths
else()
    install(TARGETS simplerasters
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${LIBRARY_NAME})
endif()