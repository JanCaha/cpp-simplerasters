find_package(GTest REQUIRED CONFIG)
include_directories(${GTEST_INCLUDE_DIRS})

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
                GTest::gtest
                GTest::gtest_main
                GTest::gmock
        )
        add_test(NAME ${testname} COMMAND ${testname})
endmacro(AddTest)

AddTest(testsinglebandraster)
AddTest(testprojectedsqurecellraster)
AddTest(testfunctions)