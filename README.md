# Campus Lab Equipment Borrowing System (100% Pure C++)

A complete, clean **100% C++ Object-Oriented Project with a 5-Entity Relational Database** designed for college lab exams, semester mini-projects, and faculty vivas.

> 📖 **FACULTY DEMO & VIVA GUIDE**: See **[HOW_TO_RUN_TEST_AND_EXPLAIN.md](HOW_TO_RUN_TEST_AND_EXPLAIN.md)** for the step-by-step 3-minute faculty demo flow and viva Q&A answers!

---

## 🚀 Quick Run on Any Computer

### Windows
```cmd
.\run.bat
```
*(Or compile directly with: `g++ main.cpp -o campus_lab.exe && .\campus_lab.exe`)*

### Linux / macOS
```bash
chmod +x run.sh
./run.sh
```

---

## 🧪 Run Automated C++ Test Suite
Compile and run the built-in C++ test suite:
```bash
g++ test_system.cpp -o test_system.exe
.\test_system.exe
```

---

## 🛠️ Technology Stack
* **Language**: 100% Standard C++ (Compatible with C++11, C++14, C++17, C++20)
* **Core Concepts**: Classes, Encapsulation, Constructors, File Streams (`<fstream>`, `<sstream>`), Delimited Database Records
* **Database Architecture**: 3NF Normalized Relational Database (`labs.txt`, `students.txt`, `equipment.txt`, `loans.txt`, `fines.txt`)
* **Dependencies**: **Zero external libraries!** Requires only a standard C++ compiler (G++, Clang, or MSVC).
