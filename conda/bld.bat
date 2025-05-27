@echo on
cmake -B build -S %SRC_DIR% -G Ninja %CMAKE_ARGS% -DBUILD_TESTS:bool=off -DPACK_DEB:bool=off

cmake --build build --config Release

cmake --install build --prefix %PREFIX%
