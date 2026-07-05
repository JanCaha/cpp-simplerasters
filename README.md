# cpp-simplerasters

**SimpleRasters** is a small C++ library that simplifies working with raster data on top of [GDAL](https://gdal.org/). It loads a single raster band into memory and provides easy cell value access, coordinate transformations between world and raster space, nodata handling, and saving back to any GDAL raster format.

## Features

- `SingleBandRaster` — loads one band of a raster file into memory, with values readable and writable as `double` regardless of the underlying GDAL data type
- `ProjectedSquareCellRaster` — a `SingleBandRaster` that additionally validates that the raster is in a projected CRS and has square cells
- world ↔ raster coordinate transformations, bounding box and point-in-raster checks
- nodata detection and handling (including rasters without a nodata value)
- comparisons of rasters: same CRS, dimensions, geotransform, data type, values
- saving to any GDAL driver that supports creation (GeoTiff by default)
- `simplerasters::rasterFormatsFileFilters()` — file dialog filters string for all GDAL raster formats

## Requirements

- C++20 compiler
- CMake ≥ 3.23 and Ninja
- GDAL (development headers)
- GTest and GMock (only for tests)

## Build and install

```bash
cmake --workflow --preset workflow-debug-tests      # build + run tests
sudo cmake --workflow --preset workflow-release-install   # build + install to /usr
```

or use the helper scripts in [scripts/](scripts/).

## Usage

```cpp
#include "simplerasters.h"

SingleBandRaster raster( "/path/to/dem.tif" );

if ( !raster.isValid() )
{
    // raster.error() holds the GDAL error message
    return 1;
}

// value at world coordinates
double v = raster.valueAt( x, y );

// value at cell (row, column)
double c = raster.value( 10, 20 );

if ( !raster.isNoData( 10, 20 ) )
{
    raster.writeValue( 10, 20, c + 1 );
}

raster.saveFile( "/path/to/output.tif" );
```

Link in CMake with:

```cmake
find_package(SimpleRasters REQUIRED)
target_link_libraries(mytarget PRIVATE SimpleRasters::simplerasters_shared)
```

(GDAL usage requirements propagate automatically; `SimpleRasters::simplerasters_static` is also available.)

## Citation

```bibtex
@Software{Caha2026,
  author  = {Jan Caha},
  title   = {simplerasters ({C++ Raster Data Access Library based on GDAL})},
  year    = {2026},
  version = {0.10.0},
  url     = {https://github.com/JanCaha/cpp-simplerasters},
}
```

## Development

Setup pre-commit hooks:

```bash
sudo apt-get install pre-commit
pre-commit install -f
```
