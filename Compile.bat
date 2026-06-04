@echo off
setlocal enabledelayedexpansion

rem --------------------------------------------------------------
rem Compile.bat - checks for CMake and C++20/Clang, then builds project
rem --------------------------------------------------------------

call :EnsureCMake
if errorlevel 1 goto End

call :EnsureClang
if errorlevel 1 goto End

call :ChooseBuildType
if errorlevel 1 goto End

call :BuildProject
if errorlevel 1 goto End

echo.
echo Build finished successfully.
goto End

:EnsureCMake
where cmake >nul 2>&1
if errorlevel 1 (
    echo.
    echo CMake was not found on your PATH.
    set /p DOWNLOAD_CMAKE=Download CMake now? [Y/N] : 
    if /I "%DOWNLOAD_CMAKE%"=="Y" (
        echo Opening CMake download page...
        powershell -Command "Start-Process 'https://cmake.org/download/'"
        echo Please install CMake, then press Enter to continue.
        pause >nul
        where cmake >nul 2>&1
        if errorlevel 1 (
            echo CMake still not found. Please install CMake and ensure it is on PATH.
            exit /b 1
        )
    ) else (
        echo CMake is required to configure and build the project.
        exit /b 1
    )
)
cmake --version
if errorlevel 1 (
    echo Failed to run cmake even though it was found.
    exit /b 1
)
exit /b 0

:EnsureClang
where clang++ >nul 2>&1
if errorlevel 1 (
    echo.
    echo Clang was not found on your PATH.
    set /p DOWNLOAD_CLANG=Download LLVM/Clang now? [Y/N] : 
    if /I "%DOWNLOAD_CLANG%"=="Y" (
        echo Opening LLVM download page...
        powershell -Command "Start-Process 'https://llvm.org/releases/download.html'"
        echo Please install LLVM/Clang, then press Enter to continue.
        pause >nul
        where clang++ >nul 2>&1
        if errorlevel 1 (
            echo Clang still not found. Please install Clang and ensure it is on PATH.
            exit /b 1
        )
    ) else (
        echo Clang is required for C++20 compilation as requested.
        exit /b 1
    )
)

set TEST_SRC=%temp%\cxx20_test.cpp
set TEST_BIN=%temp%\cxx20_test.exe
(
    echo #include <utility>
    echo #include <vector>
    echo int main() {
    echo     auto [a, b] = std::pair<int,int>{1, 2};
    echo     std::vector<int> v = {a, b};
    echo     (void)v;
    echo     return 0;
    echo }
) > "%TEST_SRC%"

clang++ -std=c++20 "%TEST_SRC%" -o "%TEST_BIN%" >nul 2>&1
if errorlevel 1 (
    echo.
    echo Clang does not appear to support C++20 on this system.
    echo Please install a newer LLVM/Clang version that supports C++20.
    exit /b 1
)

if exist "%TEST_SRC%" del /f /q "%TEST_SRC%" >nul 2>&1
if exist "%TEST_BIN%" del /f /q "%TEST_BIN%" >nul 2>&1

clang++ --version
if errorlevel 1 (
    echo Failed to run clang++ even though it was found.
    exit /b 1
)
exit /b 0

:ChooseBuildType
echo.
echo Choose build configuration:
echo   1) Debug
echo   2) Release
set /p BUILD_CHOICE=Enter 1 or 2 : 
if "%BUILD_CHOICE%"=="1" (
    set BUILD_TYPE=Debug
) else if "%BUILD_CHOICE%"=="2" (
    set BUILD_TYPE=Release
) else (
    echo Invalid selection.
    exit /b 1
)
exit /b 0

:BuildProject
set SCRIPT_DIR=%~dp0
set BUILD_DIR=%SCRIPT_DIR%build

echo.
echo Configuring project with CMake (%BUILD_TYPE%)...
cmake -S "%SCRIPT_DIR%" -B "%BUILD_DIR%" -DCMAKE_BUILD_TYPE=%BUILD_TYPE%
if errorlevel 1 (
    echo CMake configuration failed.
    exit /b 1
)

echo.
echo Building project...
cmake --build "%BUILD_DIR%" --config %BUILD_TYPE%
if errorlevel 1 (
    echo Build failed.
    exit /b 1
)
exit /b 0

:End
endlocal
