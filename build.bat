@echo off
echo Building Mp3Combiner...

if not exist "build" mkdir build
cd build

cmake ..
cmake --build . --config Release

echo.
if %ERRORLEVEL% == 0 (
    echo Build successful! Executable is in build\Release\Mp3Combiner.exe
) else (
    echo Build failed with error %ERRORLEVEL%
)

cd ..
pause
