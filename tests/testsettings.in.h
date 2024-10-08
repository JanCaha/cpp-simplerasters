#pragma once

#include <string>

#define TEST_DATA_DIR "@TEST_DATA_DIR@"
#define TEST_DATA_RESULTS_DIR "@TEST_DATA_DIR@/results"

#define TEST_DATA_DSM "@TEST_DATA_DIR@/dsm.tif"
#define TEST_DATA_DSM_4326 "@TEST_DATA_DIR@/dsm_4326.tif"
#define TEST_DATA_DSM_NON_RECTANGULAR "@TEST_DATA_DIR@/dsm_non_rectangular.tif"
#define TEST_DATA_DSM_INT32 "@TEST_DATA_DIR@/dsm_int32.tif"
#define TEST_DATA_DSM_FLOAT64 "@TEST_DATA_DIR@/dsm_float64.tif"
#define TEST_DATA_TXT "@TEST_DATA_DIR@/empty_file_for_folder_to_exist.txt"

std::string getEnvVar( std::string const &key )
{
    char *val = getenv( key.c_str() );
    return val == NULL ? std::string( "" ) : std::string( val );
}