include(FetchContent)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.13.0
)

FetchContent_GetProperties(googletest)

if(NOT googletest_POPULATED)
        FetchContent_Populate(googletest)
        add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
endif()

include_directories(
        ${GTEST_INCLUDE_DIRS}
        ${GDAL_INCLUDE_DIR}
        ${CMAKE_SOURCE_DIR}/src/api
        ${CMAKE_CURRENT_BINARY_DIR}
)

set(TEST_DATA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/data")

configure_file(testsettings.in.h testsettings.h @ONLY)

macro(AddTest testname)
        add_executable(${testname} "${testname}.cpp")
        target_link_libraries(${testname}
                ${GDAL_LIBRARY}
                simplerasters
                gtest
                gmock
                gtest_main
        )
        add_test(NAME ${testname} COMMAND ${testname})
endmacro(AddTest)

AddTest(testsinglebandraster)
AddTest(testprojectedsqurecellraster)
AddTest(testfunctions)