BASEDIR=$(dirname "$0")

cd $BASEDIR/..

rm -rf build
mkdir build

cmake \
    -S. \
    -Bbuild \
    -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_TESTS:bool=off \
    -DPACK_DEB:bool=on

cmake --build build --config Release --target all

sudo cmake --build build --config Release --target install
sudo cmake --build build --config Release --target pack_deb
