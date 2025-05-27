@echo on

cmake -B build -S %SRC_DIR% -G Ninja %CMAKE_ARGS% -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS:bool=off -DPACK_DEB:bool=off

cmake --build build --config Release

cmake --install build
