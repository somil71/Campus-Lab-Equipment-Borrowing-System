# Campus Lab Equipment Borrowing System (100% Pure C++ Syllabus & Viva Guide)

A comprehensive, normalized **100% C++ Object-Oriented Project with a 5-Entity Relational Database** designed specifically for university lab exams, semester mini-projects, and faculty vivas.

> 📖 **ER DIAGRAM SPECIFICATION**: See **[docs/ER_DIAGRAM.md](docs/ER_DIAGRAM.md)** for the complete 3NF normalization justification, table definitions, and cardinality breakdown.

---

## 🌟 Why This Project Impresses Faculty

Unlike toy 1-table or 2-table programs, this system implements a **fully normalized (3NF) relational architecture in 100% pure C++** with 5 interconnected entities:
1. **`LAB`**: Tracks physical lab rooms (e.g. *Block B - Room 304*) and faculty in-charge.
2. **`STUDENT`**: Manages university roll numbers, departments, contact info, and enforce **borrow quotas (Max 2 units)**.
3. **`EQUIPMENT`**: Tracks hardware kits, categories, assigned lab rooms, condition (*GOOD / MAINTENANCE*), and live availability.
4. **`LOAN`**: Records transactions with auto-increment IDs, issue dates, due dates, and return timestamps.
5. **`FINE`**: Automatically calculates overdue late fees (**₹10/day**) upon return and tracks payment status.

---

## 🚀 How to Run It in Terminal

### Windows (Command Prompt / PowerShell)
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

## 🧪 How to Run the Pure C++ Test Suite
Compile and run the built-in C++ test verification:
```cmd
g++ test_system.cpp -o test_system.exe
.\test_system.exe
```

---

## 📋 The 3-Minute Live Faculty Demonstration Script

Follow this sequence to showcase all 5 entities and business rules to your faculty:

| Step | Action in Program | What the Faculty Sees | What to Say |
|---|---|---|---|
| **1. Launch & Seed** | Run `campus_lab.exe` | Main menu appears with 10 options; 5 database tables are auto-verified. | *"Respected Faculty, our system uses a normalized 5-entity relational architecture written in pure C++ connecting Labs, Students, Equipment, Loans, and Late Fines."* |
| **2. Physical Lab Rooms** | Choose **Option 2** | Table showing Lab ID, Name, Physical Room Number (e.g., *Block B - Room 304*), and Faculty In-Charge. | *"Hardware is mapped to specific physical lab rooms and supervising professors."* |
| **3. Inventory & Availability** | Choose **Option 1** | Shows equipment catalog with live available stock and physical lab mapping. | *"Available units are tracked dynamically. Notice the condition status is 'GOOD'."* |
| **4. Student Borrow Quota** | Choose **Option 3** | Displays registered students and shows their current active loans vs limit (2 units). | *"To prevent equipment hoarding, each student has a strict quota limit of 2 units."* |
| **5. Issue Equipment** | Choose **Option 4** $\to$ Equipment `101`, Roll `23CS101`, Due Date | Loan confirmed with Receipt ID `#1002`. Remaining stock reduces. | *"The system checks that the student is registered, verifies their quota, and creates a loan record."* |
| **6. Test Quota Blocking** | Issue a 2nd unit to `23CS101`, then try issuing a 3rd | The 3rd attempt is rejected: `[x] REJECTED (QUOTA EXCEEDED): Student has reached limit (Max 2 units)!` | *"The business layer strictly enforces student borrow limits to ensure fair resource distribution."* |
| **7. Return & Automated Fine** | Choose **Option 5** $\to$ Enter Loan ID `1001` | Return completed. Shows whether it was on time (₹0) or overdue with late penalty (**₹10/day**). | *"Returns are timestamped. If overdue, the system automatically creates a linked fine record."* |
| **8. View & Pay Fines** | Choose **Option 7** | Shows all pending student fines with overdue days and amounts. Allows marking as `PAID`. | *"The finance/fine module tracks outstanding penalties and logs fee clearance."* |
| **9. Inspect 5 DB Files** | Choose **Option 9** | Prints the raw disk records of all 5 tables (`labs.txt`, `students.txt`, `equipment.txt`, `loans.txt`, `fines.txt`). | *"All 5 tables are permanently stored on disk with primary and foreign keys, and can also be opened in Notepad or Excel."* |

---

## 🎓 Core Viva Questions & Model Answers

### Q1: What makes your database 3NF (Third Normal Form)?
> *"Our database is fully normalized in 3NF:*
> * *1NF: Every column contains atomic values with no repeating groups.*
> * *2NF: Every non-key attribute depends entirely on the primary key.*
> * *3NF: There are no transitive dependencies. Lab room locations are stored in `labs.txt` rather than redundantly duplicated in every equipment row. Student contact details are in `students.txt` rather than duplicated in loan rows."*

### Q2: What happens if a student attempts to borrow an item under maintenance?
> *"In `issueLoan()`, the equipment condition is checked. If `condition != 'GOOD'`, the transaction is rejected with error code `-5`, preventing faulty hardware from being given to students."*

### Q3: How is the overdue fine calculated?
> *"When a return is logged, the system parses the due date and current date using `<ctime>` and `mktime()`. If `todayDate > dueDate`, it computes the day difference and applies the college late fee formula: `overdueDays * ₹10.0`."*

---

## 📁 File Structure (100% Pure C++)
```
Campus-Lab-Equipment-Borrowing-System/
├── main.cpp                        # Complete single-file C++ application (5 Entities)
├── test_system.cpp                 # Pure C++ automated test suite
├── run.bat                         # 1-click compile and run script for Windows
├── run.sh                          # 1-click compile and run script for Linux/macOS
├── labs.txt                        # Relational Table: Lab rooms and faculty
├── students.txt                    # Relational Table: Students and quotas
├── equipment.txt                   # Relational Table: Hardware inventory
├── loans.txt                       # Relational Table: Active and completed loans
├── fines.txt                       # Relational Table: Overdue penalties
├── docs/ER_DIAGRAM.md              # Complete ER diagram and 3NF specification
└── HOW_TO_RUN_TEST_AND_EXPLAIN.md  # Viva preparation and demo guide
```
