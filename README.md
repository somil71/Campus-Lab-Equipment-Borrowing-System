# Campus Lab Equipment Borrowing System

A clean, simple, and elegant **C++ Object-Oriented Project with Database Persistence** designed for college lab exams, semester mini-projects, and faculty vivas.

> 📖 **FACULTY DEMO & VIVA GUIDE**: See **[HOW_TO_RUN_TEST_AND_EXPLAIN.md](HOW_TO_RUN_TEST_AND_EXPLAIN.md)** for the step-by-step 3-minute faculty demo flow and viva Q&A answers!

---

## 🚀 Quick Run on Any Laptop

### Windows
Double-click or run from terminal:
```cmd
run.bat
```
*(Or compile directly: `g++ main.cpp -o campus_lab.exe && campus_lab.exe`)*

### Linux / macOS
```bash
chmod +x run.sh
./run.sh
```

---

## ✨ Features
1. **Inventory Management**: Add and browse lab equipment (Arduino kits, Oscilloscopes, Multimeters, Raspberry Pis, etc.).
2. **Real-time Stock Tracking**: Calculates available units (`Total - Borrowed`) dynamically.
3. **Student Lending Flow**: Issue items with Student Roll Number, Name, Department, and Return Due Date.
4. **Return & Over-Borrowing Protection**: Prevents duplicate returns and strictly blocks issuing when stock = 0.
5. **Overdue Loan Tracker**: Automatically detects and flags overdue loans.
6. **Persistent Database**: Automatically saves all tables to disk (`equipment_db.txt` and `loans_db.txt`). Open them in Notepad or use Menu Option 6 to inspect raw database rows on screen!

---

## 🛠️ Technology Stack
* **Language**: Standard C++ (compatible with C++11, C++14, C++17, C++20)
* **Concepts**: OOP (Classes, Encapsulation, Constructors, Getters/Setters), File Streams (`ifstream`, `ofstream`, `fstream`), Delimited Database Records
* **Dependencies**: **None!** Requires only a standard G++ compiler.
