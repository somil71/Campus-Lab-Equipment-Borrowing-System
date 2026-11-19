@echo off
setlocal

echo ==========================================================
echo   Building Campus Lab Equipment Borrowing System Windows
echo ==========================================================

if not exist build mkdir build

REM Check for MinGW g++ first or MSVC cl
where g++ >nul 2>&1
if %errorlevel% equ 0 goto USE_MINGW

where cl >nul 2>&1
if %errorlevel% equ 0 goto USE_MSVC

echo [x] Error: Neither MinGW (g++.exe) nor MSVC (cl.exe) found in PATH.
echo     Please install MinGW-w64 or Visual Studio C++ build tools.
exit /b 1

:USE_MINGW
echo [*] Detected MinGW G++ compiler. Compiling with G++ C++17...
if not exist build\sqlite3.o (
    echo [*] Compiling SQLite3 amalgamation one-time setup...
    gcc -c -O2 -Ivendor/sqlite vendor/sqlite/sqlite3.c -o build/sqlite3.o
)
if %errorlevel% neq 0 (
    echo [x] Failed to compile SQLite3.
    exit /b %errorlevel%
)
g++ -std=c++17 -O2 -Isrc -Ivendor/sqlite src/main.cpp src/lending.cpp src/database.cpp build/sqlite3.o -o build/campus_lab.exe
if %errorlevel% equ 0 (
    echo [OK] Build Successful: build\campus_lab.exe
) else (
    echo [x] Build Failed!
)
exit /b %errorlevel%

:USE_MSVC
echo [*] Detected MSVC compiler cl.exe. Compiling with MSVC...
if not exist build\sqlite3.obj (
    echo [*] Compiling SQLite3 amalgamation one-time setup...
    cl /c /O2 /Ivendor\sqlite vendor\sqlite\sqlite3.c /Fobuild\sqlite3.obj
)
if %errorlevel% neq 0 (
    echo [x] Failed to compile SQLite3.
    exit /b %errorlevel%
)
cl /std:c++17 /EHsc /O2 /Isrc /Ivendor\sqlite src\main.cpp src\lending.cpp src\database.cpp build\sqlite3.obj /Febuild\campus_lab.exe
if %errorlevel% equ 0 (
    echo [OK] Build Successful: build\campus_lab.exe
) else (
    echo [x] Build Failed!
)
exit /b %errorlevel%
