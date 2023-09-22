#include "api/simplerasters.h"

#include "helpers.h"

SingleBandRaster::SingleBandRaster( std::string path, GDALDataType dataType, int bandNo )
{

    setUpGDAL();

    mPath = path;

    GDALDatasetUniquePtr dataset =
        GDALDatasetUniquePtr( GDALDataset::FromHandle( GDALOpen( path.c_str(), GA_ReadOnly ) ) );

    mValid = false;
    mDataValid = false;

    if ( !dataset )
    {

        storeLastErrorMessage();
        return;
    }

    mCrs = OGRSpatialReference( *dataset->GetSpatialRef() );

    mGeoTransform = { { 0.0, 1.0, 0.0, 0.0, 0.0, 1.0 } };
    CPLErr res = GDALGetGeoTransform( dataset.get(), mGeoTransform.data() );

    if ( res != CPLErr::CE_None )
    {
        storeLastErrorMessage();
        return;
    }

    mRows = dataset->GetRasterYSize();
    mCols = dataset->GetRasterXSize();

    std::unique_ptr<GDALRasterBand> band = std::unique_ptr<GDALRasterBand>( dataset->GetRasterBand( bandNo ) );

    if ( !band )
    {
        storeLastErrorMessage();
        return;
    }

    mValid = true;

    if ( dataType == GDALDataType::GDT_Unknown )
    {
        mDataType = band->GetRasterDataType();
    }
    else
    {
        mDataType = dataType;
    }

    mNoData = band->GetNoDataValue();

    prepareDataArray();

    CPLErr ret = band->RasterIO( GDALRWFlag::GF_Read, 0, 0, mCols, mRows, mData.get(), mCols, mRows, mDataType, 0, 0 );

    if ( ret != CPLErr::CE_None )
    {
        storeLastErrorMessage();
        return;
    }

    band.release();

    mDataValid = true;
}

bool SingleBandRaster::isDataValid() const { return mDataValid; }

void SingleBandRaster::prepareDataArray()
{

    arraysize arraySize = cells();

    if ( GDALDataTypeIsFloating( mDataType ) )
    {
        if ( mDataType == GDALDataType::GDT_Float32 )
        {
            mData = std::shared_ptr<void>( new float[arraySize] );
        }
        else
        {
            mData = std::shared_ptr<void>( new double[arraySize] );
        }
    }
    else
    {
        if ( mDataType == GDALDataType::GDT_Int16 )
        {
            mData = std::shared_ptr<void>( new int16_t[arraySize] );
        }
        else if ( mDataType == GDALDataType::GDT_Int32 )
        {
            mData = std::shared_ptr<void>( new int32_t[arraySize] );
        }
        else
        {
            mData = std::shared_ptr<void>( new int64_t[arraySize] );
        }
    }
}

arraysize SingleBandRaster::toIndex( int row, int column ) const
{
    return static_cast<arraysize>( row ) * mCols + column;
}

double SingleBandRaster::noData() const { return mNoData; }

bool SingleBandRaster::isNoData( int row, int column ) const { return doubleEqual( value( row, column ), noData() ); }

bool SingleBandRaster::isNoData( arraysize index ) const { return doubleEqual( value( index ), noData() ); }

double SingleBandRaster::value( int row, int column ) const
{
    if ( row < 0 || mRows - 1 < row || column < 0 || mCols - 1 < column )
    {
        return mNoData;
    }

    return value( toIndex( row, column ) );
}

double SingleBandRaster::value( arraysize index ) const
{
    if ( !mData )
    {
        return mNoData;
    }

    if ( index < 0 || cells() - 1 < index )
    {
        return mNoData;
    }

    if ( GDALDataTypeIsFloating( mDataType ) )
    {
        if ( mDataType == GDALDataType::GDT_Float32 )
        {
            return static_cast<double>( ( static_cast<float *>( mData.get() ) )[index] );
        }
        else
        {
            return static_cast<double>( ( static_cast<double *>( mData.get() ) )[index] );
        }
    }
    else
    {
        if ( mDataType == GDALDataType::GDT_Int16 )
        {
            return static_cast<double>( ( static_cast<int16_t *>( mData.get() ) )[index] );
        }
        if ( mDataType == GDALDataType::GDT_Int32 )
        {
            return static_cast<double>( ( static_cast<int32_t *>( mData.get() ) )[index] );
        }
        else
        {
            return static_cast<double>( ( static_cast<int64_t *>( mData.get() ) )[index] );
        }
    }
}

arraysize SingleBandRaster::cells() const { return cellsInBand(); };

void SingleBandRaster::prefillValues( double value )
{
    if ( mData )
    {
        for ( size_t i = 0; i < cells(); i++ )
        {
            writeValue( i, value );
        }
    }
}

void SingleBandRaster::writeValue( int row, int column, double value )
{
    arraysize index = toIndex( row, column );
    writeValue( index, value );
}

void SingleBandRaster::writeValue( arraysize index, double value )
{
    if ( !mData )
    {
        return;
    }

    if ( GDALDataTypeIsFloating( mDataType ) )
    {
        if ( mDataType == GDALDataType::GDT_Float32 )
        {
            ( static_cast<float *>( mData.get() ) )[index] = static_cast<float>( value );
        }
        else
        {
            ( static_cast<double *>( mData.get() ) )[index] = static_cast<double>( value );
        }
    }
    else
    {
        if ( mDataType == GDALDataType::GDT_Int16 )
        {
            ( static_cast<int16_t *>( mData.get() ) )[index] = static_cast<int16_t>( value );
        }
        else if ( mDataType == GDALDataType::GDT_Int32 )
        {
            ( static_cast<int32_t *>( mData.get() ) )[index] = static_cast<int32_t>( value );
        }
        else
        {
            ( static_cast<int64_t *>( mData.get() ) )[index] = static_cast<int64_t>( value );
        }
    }
}

bool SingleBandRaster::isNoData( double row, double column ) const
{
    return isNoData( static_cast<int>( row ), static_cast<int>( column ) );
}

double SingleBandRaster::value( double row, double column ) const
{
    return value( static_cast<int>( row ), static_cast<int>( column ) );
}

double SingleBandRaster::cornerValue( double row, double column ) const
{

    if ( isNoData( row, column ) )
    {
        return mNoData;
    }

    double defaultValue = value( row, column );

    double xL = column - 0.5;
    double yL = row - 0.5;
    double xU = column + 0.5;
    double yU = row + 0.5;

    if ( isNoData( yL, xL ) || isNoData( yL, xU ) || isNoData( yU, xL ) || isNoData( yU, xU ) )
    {
        return defaultValue;
    }

    double value1, value2, value3, value4;
    value1 = value( yL, xL );
    value2 = value( yL, xU );
    value3 = value( yU, xL );
    value4 = value( yU, xU );

    return ( value1 + value2 + value3 + value4 ) / 4;
}

double SingleBandRaster::valueAt( double x, double y )
{
    double row, col;
    transformCoordinatesToRaster( x, y, row, col );
    return value( row, col );
}

bool SingleBandRaster::saveFile( std::string filename, std::string driverName )
{

    std::unique_ptr<GDALDriver> driver =
        std::unique_ptr<GDALDriver>( GDALDriver::FromHandle( GDALGetDriverByName( driverName.c_str() ) ) );

    if ( !driver )
    {
        return false;
    }

    std::unique_ptr<GDALDataset> dataset =
        std::unique_ptr<GDALDataset>( driver->Create( filename.c_str(), mCols, mRows, 1, mDataType, nullptr ) );

    if ( !dataset )
    {
        return false;
    }

    dataset->SetSpatialRef( &mCrs );
    dataset->SetGeoTransform( mGeoTransform.data() );

    std::unique_ptr<GDALRasterBand> band = std::unique_ptr<GDALRasterBand>( dataset->GetRasterBand( 1 ) );

    band->SetNoDataValue( mNoData );

    CPLErr ret = band->RasterIO( GDALRWFlag::GF_Write, 0, 0, mCols, mRows, mData.get(), mCols, mRows, mDataType, 0, 0 );

    if ( ret != CPLErr::CE_None )
    {
        return false;
    }

    band.release();

    GDALClose( dataset.get() );

    dataset.release();
    driver.release();

    return true;
}

SingleBandRaster::SingleBandRaster( const SingleBandRaster &other, GDALDataType dataType, bool copyValues )
{
    mRows = other.mRows;
    mCols = other.mCols;
    mCrs = other.mCrs;
    mGeoTransform = other.mGeoTransform;
    mNoData = other.mNoData;
    mError = other.mError;
    mValid = other.mValid;

    mDataType = dataType;

    prepareDataArray();

    if ( copyValues )
    {
        for ( size_t i = 0; i < cells(); i++ )
        {
            writeValue( i, other.value( i ) );
        }

        mDataValid = other.mDataValid;
    }
    else
    {
        prefillValues( mNoData );
        mDataValid = true;
    }
}

SingleBandRaster::SingleBandRaster( const SingleBandRaster &other, bool copyValues )
    : SingleBandRaster( other, other.mDataType, copyValues )
{
}

arraysize SingleBandRaster::dataSize() { return GDALGetDataTypeSizeBytes( mDataType ) * cells(); }