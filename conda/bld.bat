@echo on

cmake -B build -S %SRC_DIR% -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS:bool=off -DPACK_DEB:bool=off

ninja
ninja install
