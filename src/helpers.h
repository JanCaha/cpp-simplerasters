#pragma once

#include <cmath>
#include <limits>

// from QGIS
bool doubleEqual( double a, double b, double epsilon = 4 * std::numeric_limits<double>::epsilon() );