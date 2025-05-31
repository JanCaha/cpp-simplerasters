@echo on

echo Running Windows test...

if not exist "%PREFIX%/Library/bin/simplerasters.dll" exit 1
if not exist "%PREFIX%/Library/bin/simplerasters.lib" exit 1
if not exist "%PREFIX%/Library/lib\simplerasters_static.lib" exit 1
if not exist "%PREFIX%/Library/lib/cmake/SimpleRasters/SimpleRastersConfig.cmake" exit 1
if not exist "%PREFIX%/Library/include/SimpleRasters/simplerasters.h" exit 1
