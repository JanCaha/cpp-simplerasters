BASEDIR=$(dirname "$0")

cd $BASEDIR/..

rm -rf build
mkdir build

cmake \
    -S. \
    -Bbuild \
    -G Ninja \
    -DCMAKE_BUILD_TYPE=Debug \
    -DBUILD_TESTS:bool=on

cmake --build build --config Debug --target all

cd build/tests

ctest -VV
