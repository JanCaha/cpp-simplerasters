#include <cmath>
#include <utility>

#include "api/simplerasters.h"

SingleBandRaster::SingleBandRaster( SingleBandRaster &&other ) noexcept
    : AbstractRaster( std::move( other ) ), mDataType( other.mDataType ), mNoData( other.mNoData ),
      mHasNoData( other.mHasNoData )
{
    other.mValid = false;
    other.mDataValid = false;
}

SingleBandRaster &SingleBandRaster::operator=( SingleBandRaster &&other ) noexcept
{
    if ( this != &other )
    {
        AbstractRaster::operator=( std::move( other ) );
        mDataType = other.mDataType;
        mNoData = other.mNoData;
        mHasNoData = other.mHasNoData;

        other.mValid = false;
        other.mDataValid = false;
    }

    return *this;
}

SingleBandRaster::SingleBandRaster( const std::string &path, const GDALDataType dataType, const size_t bandNumber )
{

    setUpGDAL();

    GDALDatasetUniquePtr dataset =
        GDALDatasetUniquePtr( GDALDataset::FromHandle( GDALOpen( path.c_str(), GA_ReadOnly ) ) );

    mValid = false;
    mDataValid = false;

    if ( !dataset )
    {

        storeLastErrorMessage();
        return;
    }

    const OGRSpatialReference *spatialRef = dataset->GetSpatialRef();
    if ( spatialRef )
    {
        mCrs = OGRSpatialReference( *spatialRef );
    }

    mGeoTransform = { { 0.0, 1.0, 0.0, 0.0, 0.0, 1.0 } };
    CPLErr res = GDALGetGeoTransform( dataset.get(), mGeoTransform.data() );

    if ( res != CPLErr::CE_None )
    {
        storeLastErrorMessage();
        return;
    }

    mRows = dataset->GetRasterYSize();
    mCols = dataset->GetRasterXSize();

    GDALRasterBand *band = dataset->GetRasterBand( static_cast<int>( bandNumber ) );

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

    int noDataSet = 0;
    const double noDataValue = band->GetNoDataValue( &noDataSet );
    mHasNoData = noDataSet != 0;
    mNoData = mHasNoData ? noDataValue : std::numeric_limits<double>::quiet_NaN();

    prepareDataArray();

    CPLErr ret = band->RasterIO( GDALRWFlag::GF_Read, 0, 0, mCols, mRows, mData.get(), mCols, mRows,
                                 GDALDataType::GDT_Float64, 0, 0 );

    if ( ret != CPLErr::CE_None )
    {
        storeLastErrorMessage();
        return;
    }

    if ( !GDALDataTypeIsFloating( mDataType ) )
    {
        for ( std::size_t i = 0; i < cells(); i++ )
        {
            mData[i] = std::round( mData[i] );
        }
    }

    mDataValid = true;
}

bool SingleBandRaster::isDataValid() const { return mDataValid; }

void SingleBandRaster::prepareDataArray() { mData = std::make_unique<double[]>( cells() ); }

std::size_t SingleBandRaster::toIndex( int row, int column ) const
{
    return static_cast<std::size_t>( row ) * mCols + column;
}

double SingleBandRaster::noData() const { return mNoData; }

void SingleBandRaster::setNoData( double value )
{
    mNoData = value;
    mHasNoData = true;
}

bool SingleBandRaster::hasNoData() const { return mHasNoData; }

bool SingleBandRaster::isNoData( int row, int column ) const
{
    return simplerasters::compareValues( value( row, column ), noData() );
}

bool SingleBandRaster::isNoData( std::size_t index ) const
{
    return simplerasters::compareValues( value( index ), noData() );
}

double SingleBandRaster::value( int row, int column ) const
{
    if ( row < 0 || mRows - 1 < row || column < 0 || mCols - 1 < column )
    {
        return mNoData;
    }

    return value( toIndex( row, column ) );
}

double SingleBandRaster::value( std::size_t index ) const
{
    if ( !mData )
    {
        return mNoData;
    }

    if ( index >= cells() )
    {
        return mNoData;
    }

    return mData[index];
}

std::size_t SingleBandRaster::cells() const { return cellsInBand(); };

void SingleBandRaster::prefillValues( const double value )
{
    if ( mData )
    {
        for ( size_t i = 0; i < cells(); i++ )
        {
            writeValue( i, value );
        }
    }
}

void SingleBandRaster::writeValue( const int row, const int column, const double value )
{
    if ( row >= mRows || column >= mCols || row < 0 || column < 0 )
    {
        return;
    }

    std::size_t index = toIndex( row, column );
    writeValue( index, value );
}

void SingleBandRaster::writeValue( const std::size_t index, const double value )
{
    if ( !mData )
    {
        return;
    }

    if ( index >= cells() )
    {
        return;
    }

    if ( GDALDataTypeIsFloating( mDataType ) )
    {
        mData[index] = value;
    }
    else
    {
        // integer rasters store whole numbers, truncation matches the
        // previous behaviour of casting to the integer type
        mData[index] = std::trunc( value );
    }
}

bool SingleBandRaster::isNoData( double row, double column ) const
{
    // floor instead of cast, so that coordinates just outside the raster
    // (e.g. row -0.5) do not truncate towards zero into the edge cells
    return isNoData( static_cast<int>( std::floor( row ) ), static_cast<int>( std::floor( column ) ) );
}

double SingleBandRaster::value( double row, double column ) const
{
    return value( static_cast<int>( std::floor( row ) ), static_cast<int>( std::floor( column ) ) );
}

double SingleBandRaster::cornerValue( const double row, const double column ) const
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

double SingleBandRaster::valueAt( const double x, const double y ) const
{
    double row, col;
    transformCoordinatesToRaster( x, y, row, col );
    return value( row, col );
}

bool SingleBandRaster::saveFile( const std::string &filename, const std::string &driverName )
{

    GDALDriver *driver = GDALDriver::FromHandle( GDALGetDriverByName( driverName.c_str() ) );

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

    if ( !mData || !mDataValid )
    {
        return false;
    }

    dataset->SetSpatialRef( &mCrs );
    dataset->SetGeoTransform( mGeoTransform.data() );

    GDALRasterBand *band = dataset->GetRasterBand( 1 );

    if ( !band )
    {
        return false;
    }

    if ( mHasNoData )
    {
        band->SetNoDataValue( mNoData );
    }

    // buffer type is GDT_Float64 because mData stores doubles, GDAL converts
    // to the band data type (mDataType) while writing
    CPLErr ret = band->RasterIO( GDALRWFlag::GF_Write, 0, 0, mCols, mRows, mData.get(), mCols, mRows,
                                 GDALDataType::GDT_Float64, 0, 0 );

    if ( ret != CPLErr::CE_None )
    {
        return false;
    }

    return true;
}

SingleBandRaster::SingleBandRaster( const SingleBandRaster &other, const GDALDataType dataType, const bool copyValues )
{
    mRows = other.mRows;
    mCols = other.mCols;
    mCrs = other.mCrs;
    mGeoTransform = other.mGeoTransform;
    mNoData = other.mNoData;
    mHasNoData = other.mHasNoData;
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

SingleBandRaster::SingleBandRaster( const SingleBandRaster &other, const bool copyValues )
    : SingleBandRaster( other, other.mDataType, copyValues )
{
}

std::size_t SingleBandRaster::dataSize() const { return GDALGetDataTypeSizeBytes( mDataType ) * cells(); }

GDALDataType SingleBandRaster::gdalDataType() const { return mDataType; }

bool SingleBandRaster::sameDataType( const SingleBandRaster &other ) const { return mDataType == other.gdalDataType(); }

bool SingleBandRaster::sameValues( const SingleBandRaster &other, const double epsilon ) const
{
    bool same;

    for ( size_t i = 0; i < cells(); i++ )
    {
        same = simplerasters::compareValues( value( i ), other.value( i ), epsilon );

        if ( !same )
        {
            return false;
        }
    }

    return true;
}