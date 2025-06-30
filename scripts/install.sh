BASEDIR=$(dirname "$0")

cd $BASEDIR/..

rm -rf build
mkdir build

cmake \
    -S. \
    -B build \
    -G Ninja \
    -D CMAKE_C_COMPILER=clang \
    -D CMAKE_CXX_COMPILER=clang++ \
    -D CMAKE_BUILD_TYPE=Release \
    -D BUILD_TESTS:bool=off \
    -D PACK_DEB:bool=on \
    -D CMAKE_INSTALL_PREFIX=/usr

cmake --build build --config Release --target all

sudo cmake --build build --config Release --target install
sudo cmake --build build --config Release --target pack_deb
