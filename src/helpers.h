#pragma once

#include <cmath>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

#include "gdal.h"
#include "gdal_priv.h"

// registers GDAL drivers and sets up quiet error handling, guarded so that
// the initialization runs only once per process
void registerGDAL();

std::vector<std::string> metadata( GDALMajorObject *object );

std::string metadataValue( GDALMajorObject *object, const std::string key );

bool metadataEquals( GDALMajorObject *object, const std::string key, std::string expectedValue );

std::vector<std::string> splitBy( const std::string &input, const char split );

std::string fileFilter( GDALMajorObject *object );
