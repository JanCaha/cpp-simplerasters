#include "helpers.h"

bool doubleEqual( double a, double b, double epsilon )
{
    const bool aIsNan = std::isnan( a );
    const bool bIsNan = std::isnan( b );
    if ( aIsNan || bIsNan )
        return aIsNan && bIsNan;

    const double diff = a - b;
    return diff > -epsilon && diff <= epsilon;
}