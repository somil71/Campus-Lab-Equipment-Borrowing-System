#!/bin/bash
echo "=========================================================="
echo "   Campus Lab Equipment Borrowing System"
echo "=========================================================="
echo "[*] Compiling main.cpp with G++..."
g++ main.cpp -o campus_lab
if [ $? -ne 0 ]; then
    echo "[x] Compilation failed!"
    exit 1
fi
echo "[✓] Compiled successfully! Launching program..."
echo ""
./campus_lab
