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
#include <variant>

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

using VoidDeleter = void ( * )( void * );
using VoidPtr = std::unique_ptr<void, VoidDeleter>;

//////////
// Classes
//////////

class DLL_API AbstractRaster
{
  public:
    bool isValid() const;

    int xSize() const;
    int ySize() const;

    int height() const { return ySize(); };

    int width() const { return xSize(); };

    std::size_t cellsInBand() const;

    double xCellSize() const;
    double yCellSize() const;
    bool hasSquareCells() const;

    void transformCoordinatesToRaster( const double &x, const double &y, double &row, double &col ) const;
    void transformCoordinatesToWorld( const double &row, const double &col, double &x, double &y ) const;

    void transformCoordinatesToRaster( const std::shared_ptr<OGRPoint> &p, double &row, double &col ) const;
    void transformCoordinatesToWorld( const std::shared_ptr<OGRPoint> &p, double &x, double &y ) const;

    bool isInside( const std::shared_ptr<OGRPoint> &p ) const;
    bool isInside( const OGRPoint &p ) const;

    bool isProjected() const;

    std::string error() const;

    OGRSpatialReference crs() const { return mCrs; }

    OGRPolygon boundingBox() const;

    OGRPolygon extent() const { return boundingBox(); };

    bool sameDimensions( const AbstractRaster &other ) const;
    bool sameCrs( const AbstractRaster &other ) const;
    bool sameGeotransform( const AbstractRaster &other,
                           double epsilon = 4 * std::numeric_limits<double>::epsilon() ) const;

  protected:
    bool mValid = false;
    std::string mError;

    int mRows = -1;
    int mCols = -1;

    OGRSpatialReference mCrs;

    VoidPtr mData = VoidPtr( nullptr, +[]( void * ) {} );
    bool mDataValid = false;

    std::array<double, 6> mGeoTransform;

    void storeLastErrorMessage();

    void setUpGDAL();

    void bBoxCoordinates( double &minX, double &maxX, double &minY, double &maxY ) const;
};

class DLL_API SingleBandRaster : public AbstractRaster
{
  public:
    SingleBandRaster() {};
    SingleBandRaster( const std::string path, const GDALDataType dataType = GDALDataType::GDT_Unknown,
                      const size_t bandNumber = 1 );
    SingleBandRaster( const SingleBandRaster &other, const bool copyValues );
    SingleBandRaster( const SingleBandRaster &other, const GDALDataType dataType, const bool copyValues = false );

    SingleBandRaster( const SingleBandRaster & ) = delete;
    SingleBandRaster &operator=( const SingleBandRaster & ) = delete;
    SingleBandRaster( SingleBandRaster && ) = default;
    SingleBandRaster &operator=( SingleBandRaster && ) = default;

    bool isDataValid() const;

    std::size_t cells() const;

    double value( const double row, const double column ) const;
    double value( const int row, const int column ) const;
    double value( const std::size_t index ) const;

    double valueAt( const double x, const double y ) const;

    bool isNoData( const double row, const double column ) const;
    bool isNoData( const int row, const int column ) const;
    bool isNoData( const std::size_t index ) const;

    double noData() const;
    void setNoData( double value );

    double cornerValue( const double row, const double column ) const;

    void writeValue( const int row, const int column, const double value );
    void writeValue( const std::size_t index, const double value );

    void prefillValues( const double value );

    bool saveFile( const std::string filename, const std::string driverName = "GTiff" );

    std::size_t dataSize() const;

    GDALDataType gdalDataType() const;

    bool sameDataType( const SingleBandRaster &other ) const;
    bool sameValues( const SingleBandRaster &other,
                     const double epsilon = 4 * std::numeric_limits<double>::epsilon() ) const;

  protected:
    GDALDataType mDataType;

    double mNoData = std::numeric_limits<double>::quiet_NaN();

    void prepareDataArray();
    std::size_t toIndex( int row, int column ) const;
};

class DLL_API ProjectedSquareCellRaster : public SingleBandRaster
{
  public:
    ProjectedSquareCellRaster( const std::string path, GDALDataType dataType = GDALDataType::GDT_Unknown,
                               const size_t bandNumber = 1 );
    ProjectedSquareCellRaster( const SingleBandRaster &other, const bool copyValues = false );
    ProjectedSquareCellRaster( const SingleBandRaster &other, const GDALDataType dataType,
                               const bool copyValues = false );

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