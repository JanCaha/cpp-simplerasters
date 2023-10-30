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

TEST( SimpleRastersFunctions, comparison )
{
    double a, b;

    a = 0.001;
    b = 0.001;
    ASSERT_TRUE( simplerasters::compareValues( a, b ) );

    a = 5;
    b = 8;
    ASSERT_FALSE( simplerasters::compareValues( a, b ) );

    a = 0.001;
    b = 0.00101;
    ASSERT_FALSE( simplerasters::compareValues( a, b ) );
    ASSERT_TRUE( simplerasters::compareValues( a, b, 0.0001 ) );

    a = std::numeric_limits<double>::quiet_NaN();
    b = std::numeric_limits<double>::quiet_NaN();
    ASSERT_TRUE( simplerasters::compareValues( a, b ) );

    a = 1;
    b = std::numeric_limits<double>::quiet_NaN();
    ASSERT_FALSE( simplerasters::compareValues( a, b ) );
}

int main( int argc, char **argv )
{
    testing::InitGoogleTest( &argc, argv );
    return RUN_ALL_TESTS();
}