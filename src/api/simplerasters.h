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

//////////
// Classes
//////////

class AbstractRaster
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

    void transformCoordinatesToRaster( const double &x, const double &y, double &row, double &col );
    void transformCoordinatesToWorld( const double &row, const double &col, double &x, double &y );

    void transformCoordinatesToRaster( const std::shared_ptr<OGRPoint> p, double &row, double &col );
    void transformCoordinatesToWorld( const std::shared_ptr<OGRPoint> p, double &x, double &y );

    bool isInside( const std::shared_ptr<OGRPoint> p ) const;
    bool isInside( const OGRPoint p ) const;

    bool isProjected() const;

    std::string error() const;

    OGRSpatialReference crs() { return mCrs; }

    OGRPolygon boundingBox();

    OGRPolygon extent() { return boundingBox(); };

    bool sameDimensions( AbstractRaster &other );
    bool sameCrs( AbstractRaster &other );
    bool sameGeotransform( AbstractRaster &other, double epsilon = 4 * std::numeric_limits<double>::epsilon() );

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
    SingleBandRaster() {};
    SingleBandRaster( std::string path, GDALDataType dataType = GDALDataType::GDT_Unknown, int band = 1 );
    SingleBandRaster( const SingleBandRaster &other, bool copyValues = false );
    SingleBandRaster( const SingleBandRaster &other, GDALDataType dataType, bool copyValues = false );

    bool isDataValid() const;

    std::size_t cells() const;

    double value( double row, double column ) const;
    double value( int row, int column ) const;
    double value( std::size_t index ) const;

    double valueAt( double x, double y );

    bool isNoData( double row, double column ) const;
    bool isNoData( int row, int column ) const;
    bool isNoData( std::size_t index ) const;

    double noData() const;
    void setNoData( double value );

    double cornerValue( double row, double column ) const;

    void writeValue( int row, int column, double value );
    void writeValue( std::size_t index, double value );

    void prefillValues( double value );

    bool saveFile( std::string filename, std::string driverName = "GTiff" );

    std::size_t dataSize();

    GDALDataType gdalDataType();

    bool sameDataType( SingleBandRaster &other );
    bool sameValues( SingleBandRaster &other, double epsilon = 4 * std::numeric_limits<double>::epsilon() );

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
    ProjectedSquareCellRaster( std::string path, GDALDataType dataType = GDALDataType::GDT_Unknown, int band = 1 );
    ProjectedSquareCellRaster( const SingleBandRaster &other, bool copyValues = false );
    ProjectedSquareCellRaster( const SingleBandRaster &other, GDALDataType dataType, bool copyValues = false );

  private:
    void validate();
};

//////////
// Functions
//////////

namespace simplerasters
{
    std::string rasterFormatsFileFilters();

    bool compareValues( const double &a, const double &b, double epsilon = 4 * std::numeric_limits<double>::epsilon() );
} // namespace simplerasters