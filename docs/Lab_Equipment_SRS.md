# Software Requirements Specification (SRS)

## 1. Introduction
This document defines the functional and technical specifications for the **Campus Lab Equipment Borrowing System**, developed in standard **C++17** and powered by an embedded **SQLite3** relational engine.

---

## 2. System Architecture

```
+-------------------------------------------------------------+
|                  Presentation Layer                         |
|                   (src/main.cpp)                            |
|    - Terminal CLI Menu, Formatted Tables, Input Validation  |
+-------------------------------------------------------------+
                              |
                              v
+-------------------------------------------------------------+
|                  Service & Business Layer                   |
|              (src/lending.hpp, src/lending.cpp)             |
|    - LabManager: Inventory, Loans, Audit Rules, Dates       |
+-------------------------------------------------------------+
                              |
                              v
+-------------------------------------------------------------+
|               Data Access Layer (RAII C++)                  |
|             (src/database.hpp, src/database.cpp)            |
|    - Database Connection, Statement Binding, Transactions   |
+-------------------------------------------------------------+
                              |
                              v
+-------------------------------------------------------------+
|                   SQLite3 Engine (C API)                    |
|                (vendor/sqlite/sqlite3.c)                    |
|    - Single file storage, WAL mode, ACID transactions       |
+-------------------------------------------------------------+
```

---

## 3. Database Schema

### Table: `equipment`
| Column | Type | Constraints | Description |
|---|---|---|---|
| `id` | INTEGER | PRIMARY KEY AUTOINCREMENT | Unique equipment ID |
| `name` | TEXT | NOT NULL UNIQUE | Name of device/kit |
| `category` | TEXT | NOT NULL | Lab domain/category |
| `total_quantity` | INTEGER | NOT NULL CHECK(total_quantity >= 0) | Physical count |

### Table: `loans`
| Column | Type | Constraints | Description |
|---|---|---|---|
| `id` | INTEGER | PRIMARY KEY AUTOINCREMENT | Unique loan ID |
| `equipment_id` | INTEGER | NOT NULL REFERENCES equipment(id) | Foreign key to equipment |
| `student_roll` | TEXT | NOT NULL | University Roll Number |
| `student_name` | TEXT | NOT NULL | Full name of student |
| `department` | TEXT | NOT NULL | Branch / Department |
| `borrow_date` | TEXT | NOT NULL (ISO 8601) | Date issued (`YYYY-MM-DD`) |
| `due_date` | TEXT | NOT NULL (ISO 8601) | Return deadline (`YYYY-MM-DD`) |
| `return_date` | TEXT | NULLABLE (ISO 8601) | Actual return date |

---

## 4. Key Invariants & Business Logic
1. **Dynamic Availability Formula**:
   $$\text{Available} = \text{total\_quantity} - \sum (\text{loans where return\_date IS NULL})$$
2. **Double Return Immunity**: If `return_date IS NOT NULL`, any subsequent return attempt on that loan ID is rejected with an informative timestamp notification.
3. **Over-Borrowing Immunity**: An issue operation is rejected immediately if $\text{Available} \le 0$.
4. **Foreign Key Enforcement**: `PRAGMA foreign_keys = ON;` is enforced upon every connection.
