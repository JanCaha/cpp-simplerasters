#include "api/simplerasters.h"

ProjectedSquareCellRaster::ProjectedSquareCellRaster( std::string path, int bandNo ) : SingleBandRaster( path, bandNo )
{
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

ProjectedSquareCellRaster::ProjectedSquareCellRaster( const ProjectedSquareCellRaster &other, bool copyValues )
    : SingleBandRaster( other, copyValues )
{
    validate();
}

ProjectedSquareCellRaster::ProjectedSquareCellRaster( const SingleBandRaster &other, bool copyValues )
    : SingleBandRaster( other, copyValues )
{
    validate();
}