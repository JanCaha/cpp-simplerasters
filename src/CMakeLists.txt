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

# Adjust install paths for Conda on Windows
if(WIN32)
    message(STATUS "Windows detected.")

    if(DEFINED ENV{CONDA_PREFIX})
        message(STATUS "CONDA PREFIX detected adjusting install paths. Installing into $ENV{LIBRARY_PREFIX}")

        install(TARGETS simplerasters
            RUNTIME DESTINATION $ENV{LIBRARY_BIN}
            LIBRARY DESTINATION $ENV{LIBRARY_LIB}
            PUBLIC_HEADER DESTINATION $ENV{LIBRARY_INC}/${LIBRARY_NAME})
    else()
        message(FATAL_ERROR "Conda environment not detected. Please activate a Conda environment.")
    endif()

# If not Windows, use the default install paths
else()
    include(GNUInstallDirs)

    install(TARGETS simplerasters
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${LIBRARY_NAME})
endif()