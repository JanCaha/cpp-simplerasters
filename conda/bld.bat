@echo on
cmake -B build -S %SRC_DIR% -G Ninja %CMAKE_ARGS% -DBUILD_TESTS:bool=off -DPACK_DEB:bool=off
if errorlevel 1 exit 1
cmake --build build --config Release
if errorlevel 1 exit 1
cmake --install build --prefix %PREFIX%
if errorlevel 1 exit 1