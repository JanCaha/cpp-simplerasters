mkdir -p build
cmake -B build -S $SRC_DIR -G "Ninja" -DCMAKE_INSTALL_PREFIX=$PREFIX -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS:bool=off -DPACK_DEB:bool=off
cmake --build build --config Release
cmake --install build --prefix $PREFIX