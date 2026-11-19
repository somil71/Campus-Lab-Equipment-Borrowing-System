# Faculty Presentation & Viva Guide
**Campus Lab Equipment Borrowing System (C++17 + SQLite3)**

This guide is designed to help you confidently present your project to the faculty and score top marks by explaining the architecture, design decisions, C++ concepts, and live demonstration flow.

---

## 1. Quick Project Summary (Elevator Pitch for Faculty)
> *"Respected Faculty, our project is the **Campus Lab Equipment Borrowing System**. It solves a real-world problem faced by college electronics, IoT, and computer labs: tracking hardware equipment (like Arduino kits, oscilloscopes, and multimeters) loaned to students. Unlike typical academic projects that use flat text files or insecure SQL concatenation, our system is written in modern **C++17** using clean **RAII architecture**, layered 3-tier design, and an embedded **SQLite3** database with parameterized queries and ACID transactions. It guarantees zero inventory anomalies and instant overdue tracking."*

---

## 2. Why SQLite and Not MySQL or PostgreSQL?
When the faculty asks: *"Why did you use SQLite instead of a client-server DB like MySQL?"*, answer:
1. **Zero-Configuration & Portability**: SQLite requires no background server process, network daemon, or port configuration. The entire database is contained in a single standalone file (`lab_equipment.db`).
2. **Standard in Embedded & Desktop Systems**: In embedded labs, avionics, operating systems, and mobile devices (Android/iOS), SQLite is the gold standard database engine.
3. **Full ACID Compliance**: SQLite provides full transaction atomicity, consistency, isolation, and durability without external runtime dependencies.
4. **Seamless C/C++ Integration**: SQLite is native C, allowing high performance with near-zero overhead.

---

## 3. Key C++ Concepts Demonstrated
| C++ Feature | Where It Is Used in This Project | Why It Matters |
|---|---|---|
| **RAII (Resource Acquisition Is Initialization)** | `Database`, `Statement`, and `TransactionScope` classes | Prevents memory leaks and handles resource cleanup automatically when objects go out of scope. |
| **Move Semantics (`std::move`, `noexcept`)** | `Database` and `Statement` move constructors and assignment operators | Prevents accidental handle copying and enforces single ownership of database resources. |
| **Encapsulation & Layering** | `Database` (Persistence) $\to$ `LabManager` (Business logic) $\to$ `main.cpp` (Presentation) | Decouples UI from database logic, adhering to clean architecture. |
| **Rule of Five** | Deleted copy constructors, custom move constructors, explicit destructors | Enforces proper resource ownership semantics in C++. |
| **`std::optional` (C++17)** | `LabManager::getEquipmentById()` | Safely represents nullable return types without using raw nullptr. |
| **Custom Exception Handling** | `DatabaseException : public std::runtime_error` | Provides structured, descriptive error reporting across database calls. |
| **Modern STL Containers** | `std::vector`, `std::string`, `std::tuple` | High performance, memory-safe data structures. |

---

## 4. Database & SQL Concepts Demonstrated
1. **Relational Constraints & Foreign Keys**:
   - `loans.equipment_id` references `equipment.id` with `ON DELETE RESTRICT`.
   - `total_quantity >= 0` check constraint.
   - `equipment.name UNIQUE` constraint.
2. **Dynamic Inventory Calculation (Avoiding Anomalies)**:
   - Instead of maintaining a separate mutable `available_count` column that could get out of sync, available quantity is computed in SQL:
     ```sql
     SELECT e.id, e.name, e.total_quantity,
            COALESCE(COUNT(l.id), 0) AS borrowed_count
     FROM equipment e
     LEFT JOIN loans l ON e.id = l.equipment_id AND l.return_date IS NULL
     GROUP BY e.id;
     ```
3. **Defense Against SQL Injection**:
   - All user inputs are bound via parameterized placeholders (`?`) using `sqlite3_bind_text()` and `sqlite3_bind_int()`. No strings are ever concatenated into SQL commands.
4. **ACID Transactions**:
   - Multi-step operations (e.g., availability verification + loan issuance) run inside a `TransactionScope`. If anything fails, it automatically rolls back.

---

## 5. Live 3-Minute Demonstration Script

| Step | Action in Program | What to Show the Faculty | What to Say |
|---|---|---|---|
| **1. Startup & Schema** | Run `.\build\campus_lab.exe` | Main menu appears with initial seed items loaded. | *"Notice that on first launch, SQLite tables are automatically verified and seeded if empty."* |
| **2. Check Inventory** | Choose **Option 2** | Table with ID, Name, Category, Total, Borrowed, Available. | *"Here we see our lab catalog with live calculated available counts."* |
| **3. Issue an Item** | Choose **Option 3** $\to$ Select ID #1 (Arduino Kit) $\to$ Enter Roll `23CS101` $\to$ Due Date: today | Loan confirmed with unique Loan ID (e.g. #1). Remaining stock reduces by 1. | *"The system validated that stock > 0, bound student details safely, and executed an atomic transaction."* |
| **4. Deplete Stock** | Issue the remaining units of that item until Available = 0 | Attempting to issue again gives: `[!] Cannot issue: All units are currently borrowed!` | *"The business layer strictly blocks over-borrowing when available stock reaches zero."* |
| **5. Return Equipment** | Choose **Option 4** $\to$ Enter Loan ID #1 | Confirmation message. Available stock immediately increases by 1. | *"Recording the return updates the timestamp and restores unit availability instantly."* |
| **6. Prevent Double-Return** | Choose **Option 4** $\to$ Enter Loan ID #1 again | System warns: `[!] ALREADY RETURNED: Loan #1 was already returned on [Date]` | *"The system protects against human error by catching duplicate return attempts."* |
| **7. Overdue & Audit** | Choose **Option 5** and **Option 6** | Shows active loans with `OVERDUE` badge for late items, and aggregate audit stats. | *"The audit engine uses SQL aggregation to provide full transparency to the lab in-charge."* |

---

## 6. Likely Faculty Viva Questions & Answers

### Q1: What happens if the program crashes in the middle of issuing an equipment loan?
**Answer**:
> *"We implemented an RAII `TransactionScope` class. It executes `BEGIN TRANSACTION` on creation. If an exception or early exit occurs before `commit()` is called, the destructor automatically triggers `ROLLBACK`, guaranteeing that incomplete records never corrupt the database."*

### Q2: Why did you write your own C++ wrapper around SQLite instead of calling `sqlite3_exec` everywhere?
**Answer**:
> *"`sqlite3_exec` performs string concatenation which is vulnerable to SQL injection and does not easily support binary or parameterized types. Our `Statement` class wraps `sqlite3_prepare_v2` and `sqlite3_bind_*`, ensuring complete type safety, security against injection, and clean resource cleanup via `sqlite3_finalize` in the destructor."*

### Q3: How do you track whether a loan is overdue?
**Answer**:
> *"We store ISO-8601 dates (`YYYY-MM-DD`). In ISO format, chronological order matches lexicographical string order. Any active loan (`return_date IS NULL`) with `due_date < current_date` is flagged as OVERDUE, and our date arithmetic computes the exact days elapsed."*

### Q4: What design pattern did you use?
**Answer**:
> *"We utilized the **RAII pattern** for resource management, the **Repository / Service pattern** in `LabManager` to isolate business rules from raw SQL, and the **3-Tier Architecture** separating Presentation (`main.cpp`), Service (`lending.cpp`), and Data Access (`database.cpp`)."*
