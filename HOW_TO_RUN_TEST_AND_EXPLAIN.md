# Complete Guide: How to Run, Test, Show, and Explain

**Campus Lab Equipment Borrowing System**  
*Technology Stack: Modern C++17, Embedded SQLite3, Multi-Platform Build Scripts, Python Test Suite*

---

## Table of Contents
1. [Part 1: How to Run It](#part-1-how-to-run-it)
2. [Part 2: How to Test It](#part-2-how-to-test-it)
3. [Part 3: How to Show It (Faculty Live Demo Script)](#part-3-how-to-show-it-faculty-live-demo-script)
4. [Part 4: How to Explain It (Architecture, Concepts & Viva Q&A)](#part-4-how-to-explain-it-architecture-concepts--viva-qa)

---

## Part 1: How to Run It

### Prerequisites
- **Compiler**: Any standard C++17 compiler:
  - **Windows**: MinGW-w64 (GCC/G++ 9+) OR Visual Studio 2019/2022 with C++ Desktop Workload (`cl.exe`).
  - **Linux / macOS**: `g++` or `clang++` (version supporting C++17) and `make` or `cmake`.
- **Database Engine**: **No installation required!** SQLite3 is bundled as pure source code inside `vendor/sqlite/`.
- **Python (Optional)**: Python 3.8+ for running acceptance tests and demo fixture preparation.

---

### Step 1: Clone the Repository
```bash
git clone https://github.com/somil71/Campus-Lab-Equipment-Borrowing-System.git
cd Campus-Lab-Equipment-Borrowing-System
```

---

### Step 2: Build the Application

#### On Windows (PowerShell or Command Prompt)
Run the automated build batch script:
```cmd
build.bat
```
*What this does:*
1. Auto-detects whether you have MinGW `g++` or MSVC `cl.exe`.
2. Compiles the bundled SQLite amalgamation (`vendor/sqlite/sqlite3.c`) into an object file (`build\sqlite3.o` or `build\sqlite3.obj`). This happens only once and caches for ultra-fast subsequent builds.
3. Compiles the C++ source files (`src/main.cpp`, `src/lending.cpp`, `src/database.cpp`) and links them into `build\campus_lab.exe`.

#### On Linux / macOS (Terminal)
```bash
chmod +x build.sh
./build.sh
```
*What this does:* Compiles SQLite3 and the C++17 sources, linking with `-lpthread -ldl` into `./build/campus_lab`.

#### Alternative: Cross-Platform CMake Build
```bash
cmake -B build -S .
cmake --build build --config Release
```

---

### Step 3: Launch the Program

#### On Windows:
```powershell
.\build\campus_lab.exe
```

#### On Linux / macOS:
```bash
./build/campus_lab
```

*Database Storage Note:* The application creates a single, self-contained relational database file named `lab_equipment.db` in your current working directory. If you want to use a custom database file location or test file, pass `--db <path>`:
```powershell
.\build\campus_lab.exe --db my_test.db
```

---

## Part 2: How to Test It

### Automated Acceptance Testing (8/8 End-to-End Checks)
We have included a comprehensive automated acceptance test suite in [`tests/acceptance.py`](tests/acceptance.py). It creates temporary isolated database instances, executes the full lending lifecycle, checks constraints, and verifies data integrity.

Run the test suite:
```bash
python tests/acceptance.py
```
*(On Linux: `python3 tests/acceptance.py`)*

#### Expected Test Output:
```text
==========================================================
 Campus Lab Borrowing System - Acceptance Test Suite (8/8)
==========================================================
[PASS] 1. Database Schema & Integrity Constraints
[PASS] 2. Seed Data Initialization
[PASS] 3. Adding New Equipment to Inventory
[PASS] 4. Stock Calculation & Live Inventory Tracking
[PASS] 5. Lending Flow (Quantity Decrement)
[PASS] 6. Over-borrowing Prevention (Available = 0)
[PASS] 7. Duplicate Return Prevention Logic
[PASS] 8. Overdue Loan Detection & Date Arithmetic
==========================================================
 Test Results: 8/8 Passed.
==========================================================
```

### What Each Test Verifies
1. **Schema & Integrity Constraints**: Enforces foreign keys (`ON DELETE RESTRICT`), unique equipment names, and `CHECK(total_quantity >= 0)`.
2. **Seed Data Initialization**: Verifies the automatic seeding of standard lab kits if the table is empty.
3. **Adding Equipment**: Tests adding new items into inventory and retrieving their primary key IDs.
4. **Stock Calculation**: Tests dynamic SQL joining between physical inventory and active loans.
5. **Lending Flow**: Ensures stock decrements properly upon issuing.
6. **Over-Borrowing Prevention**: Confirms that when available units reach 0, borrowing is strictly blocked.
7. **Duplicate Return Prevention**: Ensures returned loans retain their initial return date and reject duplicate return attempts.
8. **Overdue Detection & Date Arithmetic**: Tests ISO-8601 date comparisons and overdue day calculations.

---

## Part 3: How to Show It (Faculty Live Demo Script)

Here is a step-by-step walkthrough to present to your faculty in **3 to 4 minutes**.

### Preparation (2 Minutes Before Demo)
Open a terminal inside the project folder:
```powershell
.\build.bat
```
*(Ensure `build\campus_lab.exe` is built and ready).*

---

### Demo Option A: Live Interactive Walkthrough (From Scratch)

Start the program:
```powershell
.\build\campus_lab.exe
```

#### Step 1: Show Initial Inventory
* **Type:** `2` and press Enter.
* **What to show:** Point out the catalog of items pre-populated into the database (Arduino Uno Kit, Digital Storage Oscilloscope, Raspberry Pi 4, etc.).
* **What to highlight:** Show the `Total`, `Borrowed`, and `Available` columns. Right now, `Borrowed` is `0` and `Available` equals `Total`.

#### Step 2: Add Custom Equipment
* **Type:** `1` and press Enter.
* **Enter:**
  * Name: `ESP32 IoT Development Kit`
  * Category: `IoT Lab`
  * Quantity: `2`
* **What to show:** `[✓] Success: Added 'ESP32 IoT Development Kit' (ID: #6) with stock of 2 units.`
* **Verify:** Type `2` to view inventory and show your new item listed in the database!

#### Step 3: Issue Equipment (First Unit)
* **Type:** `3` and press Enter.
* **Select Equipment ID:** `6` (the ESP32 Kit we just added).
* **Enter Details:**
  * Student Roll Number: `23CS001`
  * Student Name: `Aarav Sharma`
  * Department: `CSE`
  * Due Date: `2026-11-26` (or enter today's date)
* **What to show:** The system displays `[✓] LOAN CONFIRMED!`, generates Loan ID `#1`, and displays `Remaining: 1 unit(s) in lab.`

#### Step 4: Issue the Second Unit (Deplete Inventory)
* **Type:** `3` and press Enter.
* **Equipment ID:** `6`
* **Enter Details:**
  * Student Roll Number: `23CS002`
  * Student Name: `Bhavna Patel`
  * Department: `CSE`
  * Due Date: `2026-11-28`
* **What to show:** Loan ID `#2` confirmed! Remaining units: `0`.
* **Verify:** Type `2` to show stock: Total: `2`, Borrowed: `2`, Available: `0`.

#### Step 5: Test Over-Borrowing Prevention (Faculty Favorite!)
* **Type:** `3` and press Enter.
* **Select Equipment ID:** `6`
* **What to show:** The system immediately blocks the request:  
  `[!] Cannot issue: All 2 units of 'ESP32 IoT Development Kit' are currently borrowed!`
* **Explanation:** *"Sir/Ma'am, our business layer verifies available units before starting any loan transaction, preventing over-borrowing."*

#### Step 6: Return an Item
* **Type:** `4` and press Enter.
* **Enter Loan ID:** `1`
* **What to show:** `[✓] RETURN COMPLETED! Successfully returned 'ESP32 IoT Development Kit' (Student: 23CS001).`
* **Verify:** Type `2` $\to$ show that Available count for ESP32 Kit has immediately increased back to `1`!

#### Step 7: Demonstrate Duplicate Return Prevention (Edge-Case Handling)
* **Type:** `4` and press Enter.
* **Enter Loan ID:** `1` (the exact same loan we just returned).
* **What to show:**  
  `[!] ALREADY RETURNED: Loan #1 was already returned on [Date]`
* **Explanation:** *"The system tracks historic return timestamps and prevents accidental duplicate returns or inventory tampering."*

#### Step 8: View Active & Overdue Loans
* **Type:** `5` and press Enter.
* **What to show:** Loan `#2` is shown as active, displaying student roll, student name, equipment, and due date.

#### Step 9: Show Audit Statistics & Exit
* **Type:** `6` and press Enter.
* **What to show:** Full summary report showing unique items, physical inventory, active loans, and completed returns.
* **Type:** `7` to safely close the database connection and exit.

---

### Demo Option B: Instant Overdue Demo (Using Demo Fixture)
If the faculty says: *"Show me how overdue loans are handled right now"*:
1. Run the demo fixture script:
   ```powershell
   python demo\prepare_overdue.py
   ```
2. Navigate into the isolated demo folder and run the binary:
   ```powershell
   cd demo\demo_run
   & "..\..\build\campus_lab.exe"
   ```
3. Type `5` (View Active & Overdue Loans).
4. **What appears:**
   * Loan `#1` (Due 2 days ago): Highlighted as **`OVERDUE (2d)`**!
   * Loan `#2` (Due today): Highlighted as **`ACTIVE`**.
   * Loan `#3` (Due in 3 days): Highlighted as **`ACTIVE`**.

---

## Part 4: How to Explain It (Architecture, Concepts & Viva Q&A)

### The 30-Second Elevator Pitch
> *"Respected Faculty, our project is a **Campus Lab Equipment Borrowing System** written in **modern C++17** backed by an embedded **SQLite3** relational engine. It is designed for college engineering laboratories to track shared equipment loans. It features a clean **3-Tier Architecture**, complete **RAII resource management**, **parameterized SQL queries** preventing injection attacks, and **atomic ACID transactions**. Available inventory is calculated dynamically to eliminate data redundancy and concurrency anomalies."*

---

### System Architecture (The 3 Layers)

```
+-------------------------------------------------------------+
|                     PRESENTATION LAYER                      |
|                      (src/main.cpp)                         |
|  - CLI Menu, Formatted Output Tables, Robust Input Guards   |
+-------------------------------------------------------------+
                              |
                              v
+-------------------------------------------------------------+
|                  BUSINESS & SERVICE LAYER                   |
|             (src/lending.hpp, src/lending.cpp)              |
|  - LabManager: Business Invariants, Dates, Schema Seed,     |
|    Stock Calculation, Overdue Logic, Audit Summaries        |
+-------------------------------------------------------------+
                              |
                              v
+-------------------------------------------------------------+
|                 PERSISTENCE LAYER (RAII C++)                |
|            (src/database.hpp, src/database.cpp)             |
|  - Database: Connection handle ownership & Pragma controls  |
|  - Statement: Prepared statements & Parameter binding       |
|  - TransactionScope: Automatic Rollback on Exception        |
+-------------------------------------------------------------+
                              |
                              v
+-------------------------------------------------------------+
|                       SQLITE3 ENGINE                        |
|                  (vendor/sqlite/sqlite3.c)                  |
|  - Native C engine, zero-config file storage, WAL mode      |
+-------------------------------------------------------------+
```

---

### Key C++ Concepts You Must Mention

1. **RAII (Resource Acquisition Is Initialization)**:
   * *Where:* In `Database`, `Statement`, and `TransactionScope`.
   * *Why:* In C, raw SQLite handles (`sqlite3*`, `sqlite3_stmt*`) easily cause memory and file locks if not manually finalized. In our C++ design, destructors automatically call `sqlite3_finalize()` and `sqlite3_close()`.
2. **Move Semantics & Rule of Five**:
   * *Where:* `Database(const Database&) = delete;` and `Database(Database&& other) noexcept;`.
   * *Why:* Database connections cannot be shallow-copied. Move semantics allow efficient transfer of ownership without risking dangling pointers or double-free bugs.
3. **Atomic Transactions (`TransactionScope`)**:
   * *Where:* During `issueEquipment` and `returnEquipment`.
   * *Why:* Begins transaction in constructor (`BEGIN TRANSACTION;`). If an error or exception occurs before `commit()` is explicitly called, the destructor executes `ROLLBACK;`, ensuring the database is never left in a corrupted state.
4. **`std::optional<T>` (C++17)**:
   * *Where:* `getEquipmentById(int id)`.
   * *Why:* Cleanly models the possibility that an ID might not exist, avoiding unsafe NULL pointer dereferencing.
5. **Custom Exception Hierarchy**:
   * *Where:* `class DatabaseException : public std::runtime_error`.
   * *Why:* Carries both human-readable error descriptions and SQLite integer error codes across stack frames.

---

### Key Database & SQL Concepts You Must Mention

1. **Relational Schema & Foreign Keys**:
   * `equipment` table: `id` (PK), `name` (UNIQUE), `category`, `total_quantity` with `CHECK(total_quantity >= 0)`.
   * `loans` table: `equipment_id` (FK referencing `equipment(id)` with `ON DELETE RESTRICT`).
2. **Dynamic Stock Calculation (No Data Redundancy)**:
   * We do **not** store a mutable `available_quantity` column in the `equipment` table. Storing a calculated value leads to update anomalies and race conditions. Instead, we compute:
     ```sql
     SELECT e.id, e.name, e.category, e.total_quantity,
            COALESCE(COUNT(l.id), 0) AS borrowed_count
     FROM equipment e
     LEFT JOIN loans l ON e.id = l.equipment_id AND l.return_date IS NULL
     GROUP BY e.id;
     ```
3. **Zero SQL Injection Risk**:
   * We never concatenate strings (e.g. `WHERE id = " + id`). All inputs use parameterized prepared statements (`sqlite3_bind_text`, `sqlite3_bind_int`).

---

### Top 8 Viva Questions & Model Answers

#### Q1: Why did you choose SQLite over MySQL or PostgreSQL?
**Answer:**
> *"SQLite is an embedded, serverless, zero-configuration relational database engine. In academic labs and desktop applications, requiring a separate MySQL server daemon, user credentials, and network ports introduces unnecessary complexity and failure points. SQLite gives us 100% ACID compliance and high-performance SQL in a portable single-file database (`lab_equipment.db`)."*

#### Q2: How does your code protect against memory leaks with database queries?
**Answer:**
> *"We implemented the RAII idiom. The `Statement` class wraps `sqlite3_prepare_v2` in its constructor and automatically calls `sqlite3_finalize` in its destructor. Even if an exception is thrown while reading results, C++ stack unwinding guarantees that the statement destructor executes and releases the memory."*

#### Q3: What happens if two people try to borrow the last available unit at the same time?
**Answer:**
> *"All borrow operations run inside an atomic `TransactionScope`. The transaction locks the required rows, checks that `available_quantity > 0`, and inserts the loan record. If the condition fails, the transaction aborts with a descriptive error and rolls back."*

#### Q4: How is an overdue loan determined?
**Answer:**
> *"Dates are stored in standard ISO-8601 format (`YYYY-MM-DD`). In this format, lexicographical order matches chronological order. Any active loan where `return_date IS NULL` and `due_date < current_date` is mathematically overdue. Our C++ date utility converts the dates to `std::tm` and calculates the exact days overdue using `std::difftime`."*

#### Q5: What is the purpose of `COALESCE` in your SQL query?
**Answer:**
> *"`LEFT JOIN` returns `NULL` for equipment items that currently have no loan records. `COALESCE(COUNT(l.id), 0)` ensures that instead of evaluating to `NULL`, the borrowed count evaluates to integer `0`, preventing runtime null-pointer errors in C++."*

#### Q6: Why did you mark move constructors as `noexcept`?
**Answer:**
> *"In modern C++, marking move constructors as `noexcept` allows STL containers like `std::vector` to safely use move operations instead of expensive copy operations when resizing their internal memory buffers."*

#### Q7: Can a student borrow equipment if their roll number is invalid or empty?
**Answer:**
> *"No. The business logic in `LabManager::issueEquipment()` validates that the student roll number and name are non-empty strings, and verifies that the due date conforms to valid `YYYY-MM-DD` calendar rules before initiating the database transaction."*

#### Q8: How can the project be built on a computer without an IDE?
**Answer:**
> *"The project is completely build-system agnostic. It includes a native Windows batch file (`build.bat`), a Linux shell script (`build.sh`), and standard `CMakeLists.txt`. It requires only a standard C++17 compiler (GCC, Clang, or MSVC) with zero external library downloads."*
