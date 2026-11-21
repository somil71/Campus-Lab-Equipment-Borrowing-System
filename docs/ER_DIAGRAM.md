# Entity-Relationship (ER) Diagram & Relational Schema Specification

**System:** Campus Lab Equipment Borrowing System  
**Architecture:** 3-Tier Normalized Relational System (3NF)  
**Database Tables:** `labs.txt`, `students.txt`, `equipment.txt`, `loans.txt`, `fines.txt`

---

## 1. Visual Entity-Relationship Diagram (Mermaid)

```mermaid
erDiagram
    LAB ||--o{ EQUIPMENT : "houses"
    STUDENT ||--o{ LOAN : "borrows"
    EQUIPMENT ||--o{ LOAN : "issued in"
    LOAN ||--o| FINE : "incurs"

    LAB {
        string lab_id PK "Primary Key (e.g., LAB-IOT)"
        string lab_name "e.g., IoT & Robotics Lab"
        string room_no "Block B - Room 304"
        string incharge_faculty "Prof. S. K. Verma"
    }

    STUDENT {
        string roll_no PK "Primary Key (e.g., 23CS101)"
        string name "Student Full Name"
        string department "CSE / ECE / ME"
        string contact "Email or Phone"
        int max_borrow_limit "Quota limit: 2 units"
    }

    EQUIPMENT {
        int id PK "Primary Key (101, 102...)"
        string name "Equipment Name (e.g., Arduino Uno)"
        string category "Microcontrollers / Instruments"
        string lab_id FK "Foreign Key -> LAB(lab_id)"
        int total_quantity "Total physical units owned"
        int borrowed_count "Units currently out on loan"
        string condition "GOOD / MAINTENANCE"
    }

    LOAN {
        int loan_id PK "Primary Key (1001, 1002...)"
        int equipment_id FK "Foreign Key -> EQUIPMENT(id)"
        string student_roll FK "Foreign Key -> STUDENT(roll_no)"
        string issue_date "Date issued (YYYY-MM-DD)"
        string due_date "Return deadline (YYYY-MM-DD)"
        string return_date "Actual return date or PENDING"
        string status "ACTIVE / RETURNED / OVERDUE"
    }

    FINE {
        int fine_id PK "Primary Key (501, 502...)"
        int loan_id FK "Foreign Key -> LOAN(loan_id)"
        string student_roll FK "Foreign Key -> STUDENT(roll_no)"
        int overdue_days "Calculated late days"
        float fine_amount "Late penalty (₹10/day)"
        string payment_status "PENDING / PAID"
    }
```

---

## 2. Text/ASCII Diagram (For Viva Whiteboard Drawing)

```
  +------------------+
  |       LAB        |
  +------------------+
  | PK  lab_id       | 1
  |     lab_name     |
  |     room_no      |
  |     faculty      |
  +--------+---------+
           |
           | houses (1:N)
           v
  +------------------+                   +------------------+
  |    EQUIPMENT     |                   |     STUDENT      |
  +------------------+                   +------------------+
  | PK  id           | 1               1 | PK  roll_no      |
  |     name         |                   |     name         |
  |     category     |                   |     department   |
  | FK  lab_id       |                   |     contact      |
  |     total_qty    |                   |     max_limit    |
  |     borrowed_cnt |                   +--------+---------+
  |     condition    |                            |
  +--------+---------+                            | borrows (1:N)
           |                                      |
           | loaned in (1:N)                      |
           +------------------+  +----------------+
                              |  |
                              v  v
                     +-------------------+
                     |       LOAN        |
                     +-------------------+
                     | PK  loan_id       | 1
                     | FK  equipment_id  |
                     | FK  student_roll  |
                     |     issue_date    |
                     |     due_date      |
                     |     return_date   |
                     |     status        |
                     +---------+---------+
                               |
                               | incurs (1:0..1)
                               v
                     +-------------------+
                     |       FINE        |
                     +-------------------+
                     | PK  fine_id       |
                     | FK  loan_id       |
                     | FK  student_roll  |
                     |     overdue_days  |
                     |     fine_amount   |
                     |     payment_status|
                     +-------------------+
```

---

## 3. Relational Schema & Cardinalities

### 1. `LAB` $\rightarrow$ `EQUIPMENT` ($1 : N$)
* One physical lab room houses multiple equipment items.
* Every equipment item is assigned to exactly one lab.
* **Referential Integrity:** `EQUIPMENT.lab_id` references `LAB.lab_id`.

### 2. `STUDENT` $\rightarrow$ `LOAN` ($1 : N$)
* A student can borrow multiple equipment units over time.
* **Business Rule:** A student cannot exceed their `max_borrow_limit` (default: 2 active items).

### 3. `EQUIPMENT` $\rightarrow$ `LOAN` ($1 : N$)
* A single equipment kit (e.g. `Arduino Uno #101`) can have multiple borrowing records across semesters.
* **Business Rule:** Cannot issue an item if `available_quantity <= 0` or `condition == 'MAINTENANCE'`.

### 4. `LOAN` $\rightarrow$ `FINE` ($1 : 0..1$)
* On-time returns incur zero fines.
* Overdue returns automatically trigger exactly one fine record calculated at **₹10 per day late**.

---

## 4. Normalization Justification (Third Normal Form - 3NF)

When faculty asks: *"Is your database normalized?"*, answer:
1. **First Normal Form (1NF)**: Every cell contains an atomic value. No repeating groups or arrays are stored in a single column.
2. **Second Normal Form (2NF)**: All non-key attributes are fully functionally dependent on the Primary Key. (e.g. Student name depends only on `roll_no`, not on `loan_id`).
3. **Third Normal Form (3NF)**: No transitive dependencies exist. Lab room location depends on `lab_id`, not embedded inside the `equipment` table. Student contact depends on `roll_no`, not in the `loan` table.
