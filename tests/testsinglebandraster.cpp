#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "simplerasters.h"

#include "testsettings.h"

using ::testing::HasSubstr;

class SingleBandRasterTest : public ::testing::Test
{
  protected:
    SingleBandRaster r = SingleBandRaster( TEST_DATA_DSM );
};

TEST( SingleBandRaster, NotExistingFile )
{
    SingleBandRaster r = SingleBandRaster( (std::string)TEST_DATA_DIR + "/dsm_error.tif" );
    ASSERT_FALSE( r.isValid() );
    ASSERT_FALSE( r.isDataValid() );
    ASSERT_THAT( r.error(), HasSubstr( "No such file or directory" ) );
}

TEST( SingleBandRaster, NonExistingBand )
{
    SingleBandRaster r = SingleBandRaster( TEST_DATA_DSM, GDALDataType::GDT_Float32, 2 );
    ASSERT_FALSE( r.isValid() );
    ASSERT_FALSE( r.isDataValid() );
    ASSERT_THAT( r.error(), HasSubstr( "Illegal band" ) );
}

TEST( SingleBandRaster, NonRasterFile )
{
    SingleBandRaster r = SingleBandRaster( TEST_DATA_TXT );
    ASSERT_FALSE( r.isValid() );
    ASSERT_THAT( r.error(), HasSubstr( "not recognized as a supported file format" ) );
}

TEST( SingleBandRaster, Types )
{
    if ( getEnvVar( "GITHUB_ACTIONS" ).compare( "true" ) )
        GTEST_SKIP() << "Skipping this test on GitHub Actions.";

    if ( atoi( GDALVersionInfo( "VERSION_NUM" ) ) > GDAL_COMPUTE_VERSION( 3, 7, 0 ) )
    {
        std::string file = (std::string)TEST_DATA_DIR + "/dsm.tif";
        std::string resultFile = (std::string)TEST_DATA_RESULTS_DIR + "/dsm_int_32.tif";
        GDALDataType dataType = GDALDataType::GDT_Int32;

        SingleBandRaster rOrig = SingleBandRaster( file );
        ASSERT_EQ( rOrig.gdalDataType(), GDALDataType::GDT_Float32 );

        SingleBandRaster r = SingleBandRaster( file, dataType );
        ASSERT_TRUE( r.isValid() );
        ASSERT_TRUE( r.isDataValid() );
        ASSERT_EQ( r.gdalDataType(), dataType );
        r.saveFile( resultFile );

        GDALDatasetUniquePtr dataset =
            GDALDatasetUniquePtr( GDALDataset::FromHandle( GDALOpen( resultFile.c_str(), GA_ReadOnly ) ) );
        std::unique_ptr<GDALRasterBand> band = std::unique_ptr<GDALRasterBand>( dataset->GetRasterBand( 1 ) );

        ASSERT_EQ( band->GetRasterDataType(), dataType );

        // band.release();
        // dataset.release();
    }
}

TEST_F( SingleBandRasterTest, Validity )
{
    EXPECT_TRUE( r.isValid() );
    EXPECT_TRUE( r.isDataValid() );
    EXPECT_TRUE( r.isProjected() );
}

TEST_F( SingleBandRasterTest, Sizes )
{
    EXPECT_EQ( r.xSize(), 223 );
    EXPECT_EQ( r.ySize(), 175 );
    EXPECT_EQ( r.cells(), 39025 );

    SingleBandRaster r1 = SingleBandRaster( TEST_DATA_DSM_NON_RECTANGULAR );

    EXPECT_EQ( r1.xSize(), 112 );
    EXPECT_EQ( r1.ySize(), 35 );
    EXPECT_EQ( r1.cells(), 3920 );
}

TEST_F( SingleBandRasterTest, CellSizes )
{
    EXPECT_EQ( r.xCellSize(), 1 );
    EXPECT_EQ( r.yCellSize(), 1 );
    EXPECT_TRUE( r.hasSquareCells() );

    SingleBandRaster r1 = SingleBandRaster( TEST_DATA_DSM_NON_RECTANGULAR );

    EXPECT_EQ( r1.xCellSize(), 2 );
    EXPECT_EQ( r1.yCellSize(), 5 );
    EXPECT_FALSE( r1.hasSquareCells() );
}

TEST_F( SingleBandRasterTest, IsProjected )
{
    EXPECT_TRUE( r.isProjected() );

    SingleBandRaster r1 = SingleBandRaster( TEST_DATA_DSM_4326 );

    EXPECT_FALSE( r1.isProjected() );
}

TEST_F( SingleBandRasterTest, CoordinatesTransformToRaster )
{
    double x = -336531.4064;
    double y = -1189032.2883;
    double row, col;

    r.transformCoordinatesToRaster( x, y, row, col );
    EXPECT_EQ( (int)row, 0 );
    EXPECT_EQ( (int)col, 0 );

    x = -336308.7447;
    y = -1189206.9638;

    r.transformCoordinatesToRaster( x, y, row, col );
    EXPECT_EQ( (int)row, 174 );
    EXPECT_EQ( (int)col, 222 );

    double row1, col1;
    std::shared_ptr<OGRPoint> p = std::make_shared<OGRPoint>( x, y );

    r.transformCoordinatesToRaster( p, row1, col1 );
    EXPECT_EQ( row1, row );
    EXPECT_EQ( col1, col );

    // outside of raster
    // x = -336539.10;
    // y = -1189212.01;

    // r.transformCoordinatesToRaster( x, y, row, col );
}

TEST_F( SingleBandRasterTest, CoordinatesTransformToWorld )
{
    double x, y;
    double row = 0;
    double col = 0;

    r.transformCoordinatesToWorld( row, col, x, y );
    EXPECT_DOUBLE_EQ( x, -336531.64529999997 );
    EXPECT_DOUBLE_EQ( y, -1189032.1188999999 );

    row = 174;
    col = 222;

    r.transformCoordinatesToWorld( row, col, x, y );
    EXPECT_DOUBLE_EQ( x, -336309.64529999997 );
    EXPECT_DOUBLE_EQ( y, -1189206.1188999999 );
}

TEST_F( SingleBandRasterTest, NoData )
{
    EXPECT_DOUBLE_EQ( r.noData(), -3.4028230607370965e+38 );
    EXPECT_TRUE( r.isNoData( 174, 222 ) );
    EXPECT_FALSE( r.isNoData( 0, 0 ) );
}

TEST_F( SingleBandRasterTest, Value )
{
    EXPECT_DOUBLE_EQ( r.valueAt( -336531.4064, -1189032.2883 ), 1010.5443115234375 );
    EXPECT_DOUBLE_EQ( r.valueAt( -336405.15, -1189162.79 ), 1017.5416259765625 );
    EXPECT_DOUBLE_EQ( r.valueAt( -336341.28, -1189193.62 ), 987.62017822265625 );
}

TEST_F( SingleBandRasterTest, PrefillValues ) { r.prefillValues( -1 ); }

TEST_F( SingleBandRasterTest, SaveFile ) { r.saveFile( "/tmp/a.tif" ); }

TEST_F( SingleBandRasterTest, CopyRasterWithoutData )
{
    SingleBandRaster rCopy = SingleBandRaster( r );
    EXPECT_EQ( r.width(), rCopy.width() );
    EXPECT_EQ( r.height(), rCopy.height() );
    EXPECT_EQ( r.cells(), rCopy.cells() );
    EXPECT_EQ( r.isValid(), rCopy.isValid() );
    EXPECT_EQ( r.noData(), rCopy.noData() );
    EXPECT_TRUE( rCopy.isDataValid() );

    for ( size_t i = 0; i < rCopy.cells(); i++ )
    {
        EXPECT_TRUE( rCopy.isNoData( static_cast<arraysize>( i ) ) );
    }
}

TEST_F( SingleBandRasterTest, CopyRasterWithData )
{
    SingleBandRaster rCopy = SingleBandRaster( r, true );
    EXPECT_EQ( r.width(), rCopy.width() );
    EXPECT_EQ( r.height(), rCopy.height() );
    EXPECT_EQ( r.cells(), rCopy.cells() );
    EXPECT_EQ( r.isValid(), rCopy.isValid() );
    EXPECT_EQ( r.noData(), rCopy.noData() );
    EXPECT_EQ( r.isDataValid(), rCopy.isDataValid() );

    for ( size_t i = 0; i < rCopy.cells(); i++ )
    {
        EXPECT_EQ( r.value( i ), rCopy.value( i ) );
        EXPECT_EQ( r.isNoData( i ), rCopy.isNoData( i ) );
    }
}

TEST_F( SingleBandRasterTest, CopyRasterRetype )
{
    SingleBandRaster rCopy = SingleBandRaster( r, GDALDataType::GDT_Int16, true );
    ASSERT_NE( r.dataSize(), rCopy.dataSize() );
    ASSERT_GE( r.dataSize(), rCopy.dataSize() );
    ASSERT_EQ( rCopy.valueAt( -336531.4064, -1189032.2883 ), 1010 );
}

TEST_F( SingleBandRasterTest, DataSize )
{
    EXPECT_EQ( r.dataSize(), r.cells() * 4 );

    SingleBandRaster rint = SingleBandRaster( TEST_DATA_DSM_INT32 );
    EXPECT_EQ( rint.dataSize(), r.cells() * 4 );

    SingleBandRaster rfloat64 = SingleBandRaster( TEST_DATA_DSM_FLOAT64 );
    EXPECT_EQ( rfloat64.dataSize(), r.cells() * 8 );
}

TEST_F( SingleBandRasterTest, IsPointInside )
{
    OGRPoint p = OGRPoint( -336463.75, -1189114.64 );
    EXPECT_TRUE( r.isInside( p ) );

    p = OGRPoint( -336580.82, -1189006.49 );
    EXPECT_FALSE( r.isInside( p ) );

    p = OGRPoint( -336295.87, -1189221.40 );
    EXPECT_FALSE( r.isInside( p ) );
}

TEST( SingleBandRaster, EmptyRaster ) { SingleBandRaster r = SingleBandRaster(); }

TEST( SingleBandRaster, ReadDataAsDefinedType )
{
    SingleBandRaster r = SingleBandRaster( TEST_DATA_DSM, GDALDataType::GDT_Int16 );
    ASSERT_EQ( r.value( 0, 0 ), 1011 );

    r = SingleBandRaster( TEST_DATA_DSM, GDALDataType::GDT_Float64 );
    EXPECT_DOUBLE_EQ( r.value( 0, 0 ), 1010.5443115234375 );
}

TEST( SingleBandRaster, SameCRS )
{
    SingleBandRaster r = SingleBandRaster( (std::string)TEST_DATA_DIR + "/dsm.tif" );
    SingleBandRaster rInt = SingleBandRaster( (std::string)TEST_DATA_DIR + "/dsm_int32.tif" );
    SingleBandRaster rWGS84 = SingleBandRaster( (std::string)TEST_DATA_DIR + "/dsm_4326.tif" );

    ASSERT_TRUE( r.sameCrs( rInt ) );
    ASSERT_FALSE( r.sameCrs( rWGS84 ) );
}

TEST( SingleBandRaster, SameDataType )
{
    SingleBandRaster r = SingleBandRaster( (std::string)TEST_DATA_DIR + "/dsm.tif" );
    SingleBandRaster rInt = SingleBandRaster( (std::string)TEST_DATA_DIR + "/dsm_int32.tif" );
    SingleBandRaster rWGS84 = SingleBandRaster( (std::string)TEST_DATA_DIR + "/dsm_4326.tif" );

    ASSERT_TRUE( r.sameDataType( rWGS84 ) );
    ASSERT_FALSE( r.sameDataType( rInt ) );
}

TEST( SingleBandRaster, SameDimensions )
{
    SingleBandRaster r = SingleBandRaster( (std::string)TEST_DATA_DIR + "/dsm.tif" );
    SingleBandRaster rInt = SingleBandRaster( (std::string)TEST_DATA_DIR + "/dsm_int32.tif" );
    SingleBandRaster rWGS84 = SingleBandRaster( (std::string)TEST_DATA_DIR + "/dsm_4326.tif" );

    ASSERT_FALSE( r.sameDimensions( rWGS84 ) );
    ASSERT_TRUE( r.sameDimensions( rInt ) );
}

TEST( SingleBandRaster, SameValues )
{
    SingleBandRaster r = SingleBandRaster( (std::string)TEST_DATA_DIR + "/dsm.tif" );
    SingleBandRaster rInt = SingleBandRaster( (std::string)TEST_DATA_DIR + "/dsm_int32.tif" );
    SingleBandRaster rDouble = SingleBandRaster( (std::string)TEST_DATA_DIR + "/dsm_float64.tif" );

    ASSERT_FALSE( r.sameValues( rInt ) );
}

TEST( SingleBandRaster, SameGeotransform )
{
    SingleBandRaster r = SingleBandRaster( (std::string)TEST_DATA_DIR + "/dsm.tif" );
    SingleBandRaster rInt = SingleBandRaster( (std::string)TEST_DATA_DIR + "/dsm_int32.tif" );
    SingleBandRaster rWGS84 = SingleBandRaster( (std::string)TEST_DATA_DIR + "/dsm_4326.tif" );

    ASSERT_TRUE( r.sameGeotransform( rInt ) );
    ASSERT_FALSE( r.sameGeotransform( rWGS84 ) );
}

int main( int argc, char **argv )
{
    testing::InitGoogleTest( &argc, argv );
    return RUN_ALL_TESTS();
}