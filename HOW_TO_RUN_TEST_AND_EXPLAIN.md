# Campus Lab Equipment Borrowing System (College Syllabus & Viva Guide)

A simple, clean, and elegant **C++ Object-Oriented Project with Database Persistence** tailored specifically for college engineering lab exams, semester mini-projects, and faculty vivas.

---

## 🌟 Why This Project is Perfect for Your College Syllabus

Most faculty members do not want 10MB external production libraries, complex build tools, or 20 different source files. They look for:
1. **Core OOP Concepts**: Classes, Objects, Encapsulation (private variables, public getters/setters), Constructors, and Member Functions.
2. **Database & File Handling**: Using standard C++ file streams (`std::ifstream`, `std::ofstream`, `std::fstream`) to create a persistent relational database on disk (`equipment_db.txt` and `loans_db.txt`).
3. **Instant Compilation on ANY Laptop**: Compiles in **1 second** using standard G++:
   ```bash
   g++ main.cpp -o campus_lab.exe
   ```
   Works on Windows, Linux, macOS, Dev-C++, Code::Blocks, VS Code, and Visual Studio with **zero external dependencies**!
4. **Faculty Transparency**: Option 6 in the menu lets you display the raw database file records right on the terminal screen, or open them in Notepad/Excel to prove data persistence to your faculty.

---

## 🚀 How to Run It on Any Laptop

### Quick Run (Windows)
Double-click or run:
```cmd
run.bat
```
*(Or simply open Command Prompt and type: `g++ main.cpp -o campus_lab.exe && campus_lab.exe`)*

### Quick Run (Linux / macOS)
```bash
chmod +x run.sh
./run.sh
```

---

## 📋 The 3-Minute Faculty Demonstration Script

Follow this sequence when presenting to your faculty:

| Step | Action in Program | What the Faculty Sees | What to Say |
|---|---|---|---|
| **1. Launch** | Run `campus_lab.exe` | Main menu appears; database is auto-loaded or seeded. | *"Respected Faculty, this system automates equipment borrowing for campus labs using C++ OOP and file-based database persistence."* |
| **2. Check Stock** | Choose **Option 2** | Table with ID, Name, Category, Total, Borrowed, Available. | *"Here is our live lab inventory. Available stock is calculated dynamically (`Total - Borrowed`)."* |
| **3. Add Equipment** | Choose **Option 1** $\to$ Enter `ESP32 IoT Kit`, Category `IoT`, Quantity `2` | New item added with assigned ID `#106`. | *"Notice that the new equipment is saved directly to disk in `equipment_db.txt`."* |
| **4. Issue Equipment** | Choose **Option 3** $\to$ Equipment ID `106`, Roll `23CS01`, Name `Aarav`, Due Date | Loan confirmed with Receipt ID `#1`. Available stock reduces to `1`. | *"The system records student details, updates the loans database, and decrements available stock."* |
| **5. Test Stock Depletion** | Issue the 2nd unit of ID `106` | Available reaches `0`. Trying to issue again shows: `[!] All units currently on loan!` | *"The system strictly prevents over-borrowing when available stock reaches zero."* |
| **6. Return Equipment** | Choose **Option 4** $\to$ Enter Loan ID `#1` | Return confirmed! Stock increases back to `1`. | *"Returning the item updates the return date and restores the equipment back to lab stock."* |
| **7. Duplicate Return Check** | Choose **Option 4** $\to$ Enter Loan ID `#1` again | System warns: `[!] ALREADY RETURNED on [Date]!` | *"The system protects against human error by catching duplicate return attempts."* |
| **8. Overdue Loans** | Choose **Option 5** | Formatted table showing active loans with status `ACTIVE` or `OVERDUE [!]`. | *"Overdue loans are automatically identified using date comparisons to flag pending items."* |
| **9. Show Raw DB File** | Choose **Option 6** | Displays raw delimited table rows (`id|name|category|total|borrowed`). | *"You can see the persistent database files on disk (`equipment_db.txt` and `loans_db.txt`), which can also be opened in Notepad or Excel."* |

---

## 🎓 Core C++ & Database Concepts Explained (Viva Cheat Sheet)

### 1. Object-Oriented Programming (OOP)
* **Classes & Encapsulation**: 
  * `Equipment` class: Encapsulates `id`, `name`, `category`, `totalQuantity`, and `borrowedCount` as `private` data members. Access is controlled via public getters and setters.
  * `LoanRecord` class: Encapsulates student roll number, student name, department, equipment ID, borrow date, due date, and return status (`PENDING` or return date).
* **Constructors**: Parametric and default constructors initialize objects with valid state.

### 2. Database Persistence & File Handling
* **Standard C++ Streams**: Uses `std::ifstream` (reading from disk), `std::ofstream` (writing/truncating to disk), and `std::stringstream` (parsing delimited tokens).
* **Record Serialization**: Each record is stored as a delimited row (`attribute1|attribute2|...`).
  * `Equipment::toDatabaseRow()` and `Equipment::fromDatabaseRow()` convert between in-memory C++ objects and disk records.
* **Auto-Increment Primary Keys**: IDs (`101`, `102`, etc.) are generated automatically by inspecting the last record's ID.
* **Referential Integrity**: The loans table stores `equipmentId` as a foreign reference to the equipment table.

### 3. Likely Viva Questions & Answers

#### Q1: Where is the data stored? Does it persist if we close the program?
> *"Yes, the data persists permanently. It is stored in two structured database files: `equipment_db.txt` and `loans_db.txt`. When the program restarts, it reloads all records from these files using `std::ifstream`."*

#### Q2: What happens if a student tries to borrow an item that is out of stock?
> *"In `issueLoan()`, the system calculates `availableQuantity = totalQuantity - borrowedCount`. If available quantity is 0 or less, the system returns an error code `-2` and blocks the issue request."*

#### Q3: How do you prevent SQL/data corruption when returning an item?
> *"Before processing a return, `returnLoan()` checks `isReturned()`. If the return date is not `PENDING`, it blocks the transaction and warns that the loan was already closed on that date."*

#### Q4: How are dates handled?
> *"We use standard C/C++ `<ctime>` to generate today's date in `YYYY-MM-DD` format. In this ISO-8601 format, chronological comparison is identical to alphabetical comparison (`dueDate < todayDate`), allowing simple and reliable overdue detection."*

---

## 📁 File Structure
```
Campus-Lab-Equipment-Borrowing-System/
├── main.cpp                        # Complete single-file C++ application (OOP + DB)
├── run.bat                         # 1-click compile and run script for Windows
├── run.sh                          # 1-click compile and run script for Linux/macOS
├── equipment_db.txt                # Auto-generated database table for equipment
├── loans_db.txt                    # Auto-generated database table for loans
├── HOW_TO_RUN_TEST_AND_EXPLAIN.md  # Comprehensive faculty guide and viva cheat sheet
└── README.md                       # Project overview and instructions
```
