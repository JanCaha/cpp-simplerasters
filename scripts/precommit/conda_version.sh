#!/bin/bash

set -e  # Exit on any error

echo "🔍 Running conda package version update..."

VERSION=$(grep "project(" CMakeLists.txt | grep -E -o -e "[0-9\.]+")

echo -e "Project version: $VERSION"

sed -i '/version: /c\  version: \"'$VERSION'\"' conda/meta.yaml

CONDA_VERSION=$(grep "version: " conda/meta.yaml | grep -E -o -e "[0-9\.]+")

if [[ "$CONDA_VERSION" != "$VERSION" ]]; then
    echo -e "Version mismatch between CMakeLists.txt and conda/meta.yaml"
    exit 1
fi

echo -e "Conda package version set properly passed! ✨"

