#!/bin/bash
set -euo pipefail

echo "Running Unix test..."

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
check_file_exists "$PREFIX/lib/libsimplerasters.so"
check_file_exists "$PREFIX/lib/libsimplerasters.a"
check_file_exists "$PREFIX/lib/cmake/SimpleRasters/SimpleRastersConfig.cmake"
check_file_exists "$PREFIX/include/SimpleRasters/simplerasters.h"

