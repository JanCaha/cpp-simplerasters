@echo on

mkdir build

cmake -B build -S %SRC_DIR% -G "Ninja" -DCMAKE_INSTALL_PREFIX=%LIBRARY_PREFIX% -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS:bool=off -DPACK_DEB:bool=off
cmake --build build --config Release --target simplerasters_shared

REM List build output directory
dir build
dir build\src

cmake --install build --prefix %LIBRARY_PREFIX%