@echo off
setlocal

cd /d "%~dp0"

if not exist "thirdparty\imgui" (
    echo Cloning imgui ...
    git clone --depth 1 https://github.com/ocornut/imgui.git thirdparty\imgui
)

if not exist "build" mkdir build
cd build

cmake ..
if errorlevel 1 (
    echo CMake configure failed!
    pause
    exit /b 1
)

cmake --build . --config Release
if errorlevel 1 (
    echo Build failed!
    pause
    exit /b 1
)

echo.
echo Build succeeded!  Executable: build\Release\selection_demo.exe
pause
