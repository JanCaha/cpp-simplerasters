/*
SimpleRasters library - Simplified working with raster using GDAL.
MIT Licence
Copyright (C) 2023 Jan Caha
*/
#pragma once

#include <array>
#include <limits>
#include <memory>
#include <string>

#include "cpl_error.h"
#include "gdal.h"
#include "gdal_priv.h"
#include "ogr_geometry.h"

#if defined( _WIN32 ) || defined( __CYGWIN__ )
#ifdef SIMPLERASTERS_EXPORTS
#define DLL_API __declspec( dllexport )
#else
#define DLL_API __declspec( dllimport )
#endif
#else
#define DLL_API
#endif

//////////
// Classes
//////////

class AbstractRaster
{
  public:
    DLL_API bool isValid() const;

    DLL_API int xSize() const;
    DLL_API int ySize() const;

    DLL_API int height() const { return ySize(); };

    DLL_API int width() const { return xSize(); };

    DLL_API std::size_t cellsInBand() const;

    DLL_API double xCellSize() const;
    DLL_API double yCellSize() const;
    DLL_API bool hasSquareCells() const;

    DLL_API void transformCoordinatesToRaster( const double &x, const double &y, double &row, double &col );
    DLL_API void transformCoordinatesToWorld( const double &row, const double &col, double &x, double &y );

    DLL_API void transformCoordinatesToRaster( const std::shared_ptr<OGRPoint> p, double &row, double &col );
    DLL_API void transformCoordinatesToWorld( const std::shared_ptr<OGRPoint> p, double &x, double &y );

    DLL_API bool isInside( const std::shared_ptr<OGRPoint> p ) const;
    DLL_API bool isInside( const OGRPoint p ) const;

    DLL_API bool isProjected() const;

    DLL_API std::string error() const;

    DLL_API OGRSpatialReference crs() { return mCrs; }

    DLL_API OGRPolygon boundingBox();

    DLL_API OGRPolygon extent() { return boundingBox(); };

    DLL_API bool sameDimensions( AbstractRaster &other );
    DLL_API bool sameCrs( AbstractRaster &other );
    DLL_API bool sameGeotransform( AbstractRaster &other, double epsilon = 4 * std::numeric_limits<double>::epsilon() );

  protected:
    bool mValid;
    std::string mError;

    int mRows;
    int mCols;

    OGRSpatialReference mCrs;

    std::shared_ptr<void> mData = nullptr;
    bool mDataValid;

    std::array<double, 6> mGeoTransform;

    void storeLastErrorMessage();

    void setUpGDAL();

    void bBoxCoordinates( double &minX, double &maxX, double &minY, double &maxY ) const;
};

class SingleBandRaster : public AbstractRaster
{
  public:
    DLL_API SingleBandRaster() {};
    DLL_API SingleBandRaster( std::string path, GDALDataType dataType = GDALDataType::GDT_Unknown, int band = 1 );
    DLL_API SingleBandRaster( const SingleBandRaster &other, bool copyValues = false );
    DLL_API SingleBandRaster( const SingleBandRaster &other, GDALDataType dataType, bool copyValues = false );

    DLL_API bool isDataValid() const;

    DLL_API std::size_t cells() const;

    DLL_API double value( double row, double column ) const;
    DLL_API double value( int row, int column ) const;
    DLL_API double value( std::size_t index ) const;

    DLL_API double valueAt( double x, double y );

    DLL_API bool isNoData( double row, double column ) const;
    DLL_API bool isNoData( int row, int column ) const;
    DLL_API bool isNoData( std::size_t index ) const;

    DLL_API double noData() const;
    DLL_API void setNoData( double value );

    DLL_API double cornerValue( double row, double column ) const;

    DLL_API void writeValue( int row, int column, double value );
    DLL_API void writeValue( std::size_t index, double value );

    DLL_API void prefillValues( double value );

    DLL_API bool saveFile( std::string filename, std::string driverName = "GTiff" );

    DLL_API std::size_t dataSize();

    DLL_API GDALDataType gdalDataType();

    DLL_API bool sameDataType( SingleBandRaster &other );
    DLL_API bool sameValues( SingleBandRaster &other, double epsilon = 4 * std::numeric_limits<double>::epsilon() );

  protected:
    GDALDataType mDataType;

    std::shared_ptr<void> mData = nullptr;
    bool mDataValid;

    double mNoData = std::numeric_limits<double>::quiet_NaN();

    void prepareDataArray();
    std::size_t toIndex( int row, int column ) const;
};

class ProjectedSquareCellRaster : public SingleBandRaster
{
  public:
    DLL_API ProjectedSquareCellRaster( std::string path, GDALDataType dataType = GDALDataType::GDT_Unknown,
                                       int band = 1 );
    DLL_API ProjectedSquareCellRaster( const SingleBandRaster &other, bool copyValues = false );
    DLL_API ProjectedSquareCellRaster( const SingleBandRaster &other, GDALDataType dataType, bool copyValues = false );

  private:
    void validate();
};

//////////
// Functions
//////////

namespace simplerasters
{
    DLL_API std::string rasterFormatsFileFilters();

    DLL_API bool compareValues( const double &a, const double &b,
                                double epsilon = 4 * std::numeric_limits<double>::epsilon() );
} // namespace simplerasters