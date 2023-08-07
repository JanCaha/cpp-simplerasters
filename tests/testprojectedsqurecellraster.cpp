#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "simplerasters.h"

#include "testsettings.h"

using ::testing::HasSubstr;

TEST( ProjectedSquareCellRaster, NotExistingFile )
{
    ProjectedSquareCellRaster r = ProjectedSquareCellRaster( (std::string)TEST_DATA_DIR + "/dsm_error.tif" );
    ASSERT_FALSE( r.isValid() );
    ASSERT_FALSE( r.isDataValid() );
    ASSERT_THAT( r.error(), HasSubstr( "No such file or directory" ) );
}

TEST( ProjectedSquareCellRaster, NonRectangular )
{
    ProjectedSquareCellRaster r = ProjectedSquareCellRaster( TEST_DATA_DSM_NON_RECTANGULAR );
    ASSERT_FALSE( r.isValid() );
    ASSERT_FALSE( r.isDataValid() );
    ASSERT_THAT( r.error(), HasSubstr( "Non square cells not allowed" ) );
}

TEST( ProjectedSquareCellRaster, NotProjected )
{
    ProjectedSquareCellRaster r = ProjectedSquareCellRaster( TEST_DATA_DSM_4326 );
    ASSERT_FALSE( r.isValid() );
    ASSERT_FALSE( r.isDataValid() );
    ASSERT_THAT( r.error(), HasSubstr( "Raster is not projected" ) );
}

TEST( ProjectedSquareCellRaster, FromSingleBandRaster )
{
    SingleBandRaster rSingleBand = SingleBandRaster( TEST_DATA_DSM );
    ProjectedSquareCellRaster r = ProjectedSquareCellRaster( rSingleBand );
    ASSERT_TRUE( r.isValid() );
}

int main( int argc, char **argv )
{
    testing::InitGoogleTest( &argc, argv );
    return RUN_ALL_TESTS();
}