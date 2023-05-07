#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "simplerasters.h"

TEST( SimpleRastersFunctions, rasterFormatsFileFilters )
{
    std::string fileFilter = simplerasters::rasterFormatsFileFilters();

    std::string ext = "tif";
    ASSERT_TRUE( fileFilter.find( ext ) > 0 && fileFilter.find( ext ) < std::string::npos );

    ext = "jpg";
    ASSERT_TRUE( fileFilter.find( ext ) > 0 && fileFilter.find( ext ) < std::string::npos );

    ext = "dem";
    ASSERT_TRUE( fileFilter.find( ext ) > 0 && fileFilter.find( ext ) < std::string::npos );

    ext = "sdat";
    ASSERT_TRUE( fileFilter.find( ext ) > 0 && fileFilter.find( ext ) < std::string::npos );
}

int main( int argc, char **argv )
{
    testing::InitGoogleTest( &argc, argv );
    return RUN_ALL_TESTS();
}