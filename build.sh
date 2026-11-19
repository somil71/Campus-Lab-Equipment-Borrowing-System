#!/bin/sh
set -e

echo "=========================================================="
echo "   Building Campus Lab Equipment Borrowing System (Linux) "
echo "=========================================================="

mkdir -p build

if [ ! -f build/sqlite3.o ]; then
    echo "[*] Compiling SQLite3 amalgamation (one-time setup)..."
    gcc -c -O2 -Ivendor/sqlite vendor/sqlite/sqlite3.c -o build/sqlite3.o
fi

echo "[*] Compiling C++ application with G++ (C++17)..."
g++ -std=c++17 -O2 -Isrc -Ivendor/sqlite \
    src/main.cpp src/lending.cpp src/database.cpp build/sqlite3.o \
    -o build/campus_lab -lpthread -ldl

echo "[✓] Build Successful: ./build/campus_lab"
