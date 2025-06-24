#!/bin/bash
set -euo pipefail

echo "Running Unix test..."

# change dynamic library extension based on OS - so for Linux, dylib for macOS
OS_NAME="$(uname -s)"
LIBRARY_EXTENSION="so"
if [[ "$OS_NAME" == "Darwin" ]]; then
    LIBRARY_EXTENSION="dylib"
fi

# Function to check if a file exists
check_file_exists() {
  local filepath="$1"
  if [[ -f "$filepath" ]]; then
    echo "✅ File exists: $filepath"
  else
    echo "❌ Missing file: $filepath"
    exit 1
  fi
}

# existence of files
check_file_exists "$PREFIX/lib/libsimplerasters.$LIBRARY_EXTENSION"
check_file_exists "$PREFIX/lib/libsimplerasters.a"
check_file_exists "$PREFIX/lib/cmake/SimpleRasters/SimpleRastersConfig.cmake"
check_file_exists "$PREFIX/include/SimpleRasters/simplerasters.h"

