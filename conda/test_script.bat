@echo on

echo Running Windows test...

setlocal EnableDelayedExpansion

REM Define a function to check file existence
:check_file_exists
set FILE=%1
if exist "!FILE!" (
    echo ✅ File exists: !FILE!
) else (
    echo ❌ Missing file: !FILE!
    exit /b 1
)
goto :eof

REM Use the function to test required files

call :check_file_exists "%PREFIX%\Library\bin\simplerasters.dll"
@REM call :check_file_exists "%PREFIX%\Library\bin\simplerasters.lib"

REM Add more checks as needed
endlocal