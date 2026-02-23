find_package(GTest REQUIRED CONFIG)
include(GoogleTest)

set(TEST_DATA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/data")

configure_file(testsettings.in.h testsettings.h @ONLY)

macro(add_library_test testname)
        add_executable(${testname} "${testname}.cpp")
        target_compile_features(${testname} PRIVATE cxx_std_20)
        target_include_directories(${testname}
                PRIVATE
                ${CMAKE_SOURCE_DIR}/src/api
                ${CMAKE_CURRENT_BINARY_DIR}
        )

        target_link_libraries(${testname}
                PRIVATE
                simplerasters_shared
                GDAL::GDAL
                GTest::gtest_main
        )

        gtest_discover_tests(${testname})
endmacro(add_library_test)

add_library_test(testsinglebandraster)
add_library_test(testprojectedsqurecellraster)
add_library_test(testfunctions)