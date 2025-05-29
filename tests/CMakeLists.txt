find_package(GTest REQUIRED CONFIG)

include_directories(
        ${GTEST_INCLUDE_DIRS}
        ${GDAL_INCLUDE_DIR}
        ${CMAKE_SOURCE_DIR}/src/api
        ${CMAKE_CURRENT_BINARY_DIR}
)

set(TEST_DATA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/data")

configure_file(testsettings.in.h testsettings.h @ONLY)

macro(add_library_test testname)
        add_executable(${testname} "${testname}.cpp")
        target_link_libraries(${testname}
                ${GDAL_LIBRARY}
                simplerasters_shared
                GTest::gtest
                GTest::gtest_main
                GTest::gmock
        )
        add_test(NAME ${testname} COMMAND ${testname})
endmacro(add_library_test)

add_library_test(testsinglebandraster)
add_library_test(testprojectedsqurecellraster)
add_library_test(testfunctions)