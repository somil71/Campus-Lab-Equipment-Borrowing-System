@echo off
echo ==========================================================
echo   Campus Lab Equipment Borrowing System
echo ==========================================================
echo [*] Compiling main.cpp with G++...
g++ main.cpp -o campus_lab.exe
if %errorlevel% neq 0 (
    echo [x] Compilation failed! Ensure g++ is on PATH.
    pause
    exit /b %errorlevel%
)
echo [✓] Compiled successfully! Launching program...
echo.
campus_lab.exe
