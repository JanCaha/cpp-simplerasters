#include "api/simplerasters.h"

ProjectedSquareCellRaster::ProjectedSquareCellRaster( const std::string path, GDALDataType dataType,
                                                      const size_t bandNumber )
    : SingleBandRaster( path, dataType, bandNumber )
{
    if ( !isValid() )
    {
        return;
    }
    validate();
}

void ProjectedSquareCellRaster::validate()
{
    mValid = false;
    mDataValid = false;

    if ( !hasSquareCells() )
    {
        mError = "Non square cells not allowed.";
        return;
    }

    if ( !isProjected() )
    {
        mError = "Raster is not projected.";
        return;
    }

    mDataValid = true;
    mValid = true;
}

ProjectedSquareCellRaster::ProjectedSquareCellRaster( const SingleBandRaster &other, const bool copyValues )
    : SingleBandRaster( other, copyValues )
{
    if ( !isValid() )
    {
        return;
    }
    validate();
}

ProjectedSquareCellRaster::ProjectedSquareCellRaster( const SingleBandRaster &other, const GDALDataType dataType,
                                                      const bool copyValues )
    : SingleBandRaster( other, dataType, copyValues )
{
    if ( !isValid() )
    {
        return;
    }
    validate();
}