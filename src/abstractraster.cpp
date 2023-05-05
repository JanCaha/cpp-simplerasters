#include "api/simplerasters.h"

#include "helpers.h"

int AbstractRaster::xSize() const { return mCols; }

int AbstractRaster::ySize() const { return mRows; }

u_long AbstractRaster::cellsInBand() const { return mRows * mCols; }

double AbstractRaster::xCellSize() const { return abs( mGeoTransform[1] ); }

double AbstractRaster::yCellSize() const { return abs( mGeoTransform[5] ); }

bool AbstractRaster::hasSquareCells() const { return doubleEqual( xCellSize(), yCellSize() ); }

void AbstractRaster::storeLastErrorMessage()
{
    mError = CPLGetLastErrorMsg();
    CPLErrorReset();
}

std::string AbstractRaster::error() const { return mError; }

void AbstractRaster::transformCoordinatesToRaster( const double &x, const double &y, double &row, double &col )
{
    double div = ( mGeoTransform[2] * mGeoTransform[4] - mGeoTransform[1] * mGeoTransform[5] );
    col =
        -( mGeoTransform[2] * ( mGeoTransform[3] - y ) + mGeoTransform[5] * x - mGeoTransform[0] * mGeoTransform[5] ) /
        div;
    row = ( mGeoTransform[1] * ( mGeoTransform[3] - y ) + mGeoTransform[4] * x - mGeoTransform[0] * mGeoTransform[4] ) /
          div;
}

void AbstractRaster::transformCoordinatesToRaster( const std::shared_ptr<OGRPoint> p, double &row, double &col )
{
    transformCoordinatesToRaster( p->getX(), p->getY(), row, col );
}

void AbstractRaster::transformCoordinatesToWorld( const double &row, const double &col, double &x, double &y )
{
    x = mGeoTransform[0] + mGeoTransform[1] * col + mGeoTransform[2] * row;
    y = mGeoTransform[3] + mGeoTransform[4] * col + mGeoTransform[5] * row;
}

void AbstractRaster::transformCoordinatesToWorld( const std::shared_ptr<OGRPoint> p, double &x, double &y )
{
    transformCoordinatesToRaster( p->getY(), p->getX(), x, y );
}

bool AbstractRaster::isValid() const { return mValid; }

bool AbstractRaster::isProjected() const { return mCrs.IsProjected(); };

void AbstractRaster::setUpGDAL()
{
    // register all drivers
    GDALAllRegister();

    // do not print errors
    CPLPushErrorHandler( CPLQuietErrorHandler );
}

bool AbstractRaster::isInside( const std::shared_ptr<OGRPoint> p ) const { return isInside( *p.get() ); }

void AbstractRaster::bBoxCoordinates( double &minX, double &maxX, double &minY, double &maxY ) const
{
    minX = mGeoTransform[0];
    minY = mGeoTransform[3];

    maxX = minX + ( width() * mGeoTransform[1] );
    maxY = minY + ( height() * mGeoTransform[5] );

    double tmp;

    if ( maxX < minX )
    {
        tmp = minX;
        minX = maxX;
        maxX = tmp;
    }

    if ( maxY < minY )
    {
        tmp = minY;
        minY = maxY;
        maxY = tmp;
    }
}

bool AbstractRaster::isInside( const OGRPoint p ) const
{
    double minX, maxX, minY, maxY;

    bBoxCoordinates( minX, maxX, minY, maxY );

    if ( minX <= p.getX() && p.getX() <= maxX && minY <= p.getY() && p.getY() <= maxY )
    {
        return true;
    }

    return false;
}

OGRPolygon AbstractRaster::boundingBox()
{
    double minX, maxX, minY, maxY;

    bBoxCoordinates( minX, maxX, minY, maxY );

    OGRLinearRing ring;
    ring.addPoint( minX, minY );
    ring.addPoint( maxX, minY );
    ring.addPoint( maxX, maxY );
    ring.addPoint( minX, maxY );
    ring.addPoint( minX, minY );

    OGRPolygon poly = OGRPolygon();
    poly.addRing( &ring );

    return poly;
}