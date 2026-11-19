# Product Requirements Document (PRD)

## Project Title
**Campus Lab Equipment Borrowing System**

---

## 1. Problem Statement
Academic laboratories manage shared hardware resources (e.g., microcontrollers, oscilloscopes, sensors, multimeters, and FPGA boards). In typical colleges, equipment issuing is managed via manual physical registers or basic spreadsheets, leading to:
1. **Uncertain Inventory**: Lab assistants cannot instantly see real-time available stock versus loaned items.
2. **Missing Returns**: Inability to identify overdue items and student borrowers.
3. **Ghost Stock & Data Redundancy**: Static count columns get corrupted when parallel borrows or returns happen.

## 2. Product Objectives
- Provide a clean, robust, zero-configuration C++ terminal application for campus lab assistants.
- Maintain a transactional, relational database (SQLite) embedded directly in the application.
- Guarantee inventory integrity through dynamic SQL computations (`available = total - active_loans`).
- Prevent double returns, over-borrowing, and SQL injections via parameterized queries.

## 3. Target Users
- **Lab Assistants / Technicians**: Managing physical handovers, tracking serial loans, verifying returns.
- **Faculty / Lab In-Charge**: Monitoring asset utilization, lab audits, and overdue items.
- **Students**: Borrowing lab components for course projects and practical exams.

## 4. Key Functional Features
1. **Equipment Catalog Management**: Add hardware items, categories, and initial quantities.
2. **Real-Time Stock Querying**: Compute available vs borrowed units dynamically without data anomalies.
3. **Student Lending Workflow**: Issue equipment to a student with roll number, department, and return deadline.
4. **Return Processing**: Mark loans returned, update timestamps, and prevent accidental double-returns.
5. **Overdue Loan Tracker**: Automatically detect loans where `due_date < current_date` and alert lab staff.
6. **Lab Audit Summary**: Aggregate metrics on total units, active loans, and return counts.

## 5. Non-Functional Requirements
- **Performance**: Instant command execution (< 10ms for all database operations).
- **Portability**: Standalone zero-dependency build on Windows and Linux using C++17 standard.
- **Reliability & ACID**: Atomic transactions to protect inventory during multi-step borrow operations.
