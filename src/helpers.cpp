#include <algorithm>
#include <cctype>
#include <cmath>
#include <iostream>
#include <mutex>
#include <regex>
#include <sstream>

#include "api/simplerasters.h"
#include "helpers.h"

void registerGDAL()
{
    static std::once_flag gdalInitFlag;

    std::call_once( gdalInitFlag,
                    []()
                    {
                        GDALAllRegister();
                        CPLPushErrorHandler( CPLQuietErrorHandler );
                    } );
}

std::vector<std::string> metadata( GDALMajorObject *object )
{
    CSLConstList metadata = object->GetMetadata();
    std::vector<std::string> strings;

    if ( metadata )
    {
        while ( *metadata )
        {
            strings.emplace_back( *metadata );
            metadata++;
        }
    }

    return strings;
}

std::string metadataValue( GDALMajorObject *object, const std::string key )
{
    const std::string prefix = key + "=";
    std::vector<std::string> strings = metadata( object );

    for ( auto const &str : strings )
    {
        if ( str.rfind( prefix, 0 ) == 0 )
        {
            return str.substr( prefix.length() );
        }
    }
    return std::string();
}

bool metadataEquals( GDALMajorObject *object, const std::string key, std::string expectedValue )
{
    std::string realValue = metadataValue( object, key );
    std::transform( realValue.begin(), realValue.end(), realValue.begin(),
                    []( unsigned char c ) { return static_cast<char>( std::tolower( c ) ); } );

    std::transform( expectedValue.begin(), expectedValue.end(), expectedValue.begin(),
                    []( unsigned char c ) { return static_cast<char>( std::tolower( c ) ); } );

    return realValue == expectedValue;
}

std::vector<std::string> splitBy( const std::string &input, const char split )
{
    std::vector<std::string> strings;

    std::istringstream f( input );
    std::string s;

    while ( std::getline( f, s, split ) )
    {
        strings.push_back( s );
    }

    return strings;
}

std::string fileFilter( GDALMajorObject *object )
{
    std::string name = metadataValue( object, GDAL_DMD_LONGNAME );

    std::regex name_part( "\\(.+\\)" );

    name = std::regex_replace( name, name_part, "" );
    name = std::regex_replace( name, std::regex( "\\s+$" ), "" );

    std::string extensions = metadataValue( object, GDAL_DMD_EXTENSIONS );

    std::vector<std::string> exts = splitBy( extensions, ' ' );

    if ( exts.empty() )
    {
        return "";
    }

    std::string res = name + " (";
    for ( size_t i = 0; i < exts.size(); i++ )
    {
        if ( i != 0 )
        {
            res += " ";
        }
        res += "*." + exts[i];
    }
    res += ")";

    return res;
}

std::string simplerasters::rasterFormatsFileFilters()
{
    GDALAllRegister();

    std::string completeFileFilter;
    GDALDriverManager *dm = GetGDALDriverManager();

    int count = dm->GetDriverCount();
    GDALDriver *driver;
    for ( int i = 0; i < count; i++ )
    {
        driver = dm->GetDriver( i );

        if ( metadataEquals( driver, "DCAP_RASTER", "YES" ) )
        {
            std::string driverFileFilter = fileFilter( driver );
            if ( !driverFileFilter.empty() )
            {
                if ( completeFileFilter.empty() )
                {
                    completeFileFilter += driverFileFilter;
                }
                else
                {
                    completeFileFilter += ";;" + driverFileFilter;
                }
            }
        }
    }

    return completeFileFilter;
}

bool simplerasters::compareValues( const double &a, const double &b, double epsilon )
{
    if ( std::isnan( a ) && std::isnan( b ) )
    {
        return true;
    }

    if ( a == b )
    {
        return true;
    }

    // scale the tolerance to the magnitude of the compared values, otherwise
    // the default epsilon is meaningless for values far from 1 (e.g. nodata
    // -3.4e+38 or projected coordinates)
    const double scale = std::max( { 1.0, std::abs( a ), std::abs( b ) } );

    return std::abs( a - b ) <= epsilon * scale;
}