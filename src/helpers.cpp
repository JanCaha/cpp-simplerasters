#include <algorithm>
#include <cctype>
#include <iostream>
#include <regex>

#include "api/simplerasters.h"
#include "helpers.h"

std::vector<std::string> metadata( GDALMajorObject *object )
{
    char **metadata = object->GetMetadata();
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

std::vector<std::string> splitBy( std::string input, const char *split )
{
    std::vector<std::string> strings;

    std::istringstream f( input );
    std::string s;

    while ( std::getline( f, s, *split ) )
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

    std::vector<std::string> exts = splitBy( extensions, " " );

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
    for ( size_t i = 0; i < count; i++ )
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

    if ( std::abs( a - b ) < epsilon )
    {
        return true;
    }

    return false;
}