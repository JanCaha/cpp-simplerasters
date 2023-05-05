BASEDIR=$(dirname "$0")

cd $BASEDIR/..

rm -rf build
mkdir build

cmake \
    -S. \
    -Bbuild \
    -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_TESTS:bool=off

cmake --build build --config Release --target all

sudo cmake --build build --config Release --target install
