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
// Types
//////////

typedef u_long arraysize;

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

    u_long cellsInBand() const;

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
    SingleBandRaster(){};
    SingleBandRaster( std::string path, int band = 1 );
    SingleBandRaster( const SingleBandRaster &other, bool copyValues = false );

    bool isDataValid() const;

    u_long cells() const;

    double value( double row, double column ) const;
    double value( int row, int column ) const;
    double value( arraysize index ) const;

    double valueAt( double x, double y );

    bool isNoData( double row, double column ) const;
    bool isNoData( int row, int column ) const;
    bool isNoData( arraysize index ) const;

    double noData() const;

    double cornerValue( double row, double column ) const;

    void writeValue( int row, int column, double value );
    void writeValue( arraysize index, double value );

    void prefillValues( double value );

    bool saveFile( std::string filename, std::string driverName = "GTiff" );

    u_long dataSize();

  protected:
    GDALDataType mDataType;

    std::shared_ptr<void> mData = nullptr;
    bool mDataValid;

    double mNoData = std::numeric_limits<double>::quiet_NaN();

    void prepareDataArray();
    arraysize toIndex( int row, int column ) const;
};

class ProjectedSquareCellRaster : public SingleBandRaster
{
  public:
    ProjectedSquareCellRaster( std::string path, int band = 1 );
    ProjectedSquareCellRaster( const ProjectedSquareCellRaster &other, bool copyValues = false );
    ProjectedSquareCellRaster( const SingleBandRaster &other, bool copyValues = false );

  private:
    void validate();
};

//////////
// Functions
//////////

std::string rasterFilters();