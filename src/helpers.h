#pragma once

#include <cmath>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

#include "gdal.h"
#include "gdal_priv.h"

std::vector<std::string> metadata( GDALMajorObject *object );

std::string metadataValue( GDALMajorObject *object, const std::string key );

bool metadataEquals( GDALMajorObject *object, const std::string key, std::string expectedValue );

std::vector<std::string> splitBy( std::string input, const char *split );

std::string fileFilter( GDALMajorObject *object );
