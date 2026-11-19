#!/usr/bin/env python3
"""
Campus Lab Equipment Borrowing System - Interactive Runner
This script provides the exact same menu, queries, transactions, and business logic
as the C++ application (src/main.cpp, src/lending.cpp, src/database.cpp)
connecting directly to the SQLite database (lab_equipment.db).
"""

import sys
import os
import sqlite3
import datetime

DB_PATH = "lab_equipment.db"

def init_db(db_path):
    conn = sqlite3.connect(db_path)
    cur = conn.cursor()
    cur.executescript("""
    PRAGMA foreign_keys = ON;
    CREATE TABLE IF NOT EXISTS equipment (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        name TEXT NOT NULL UNIQUE,
        category TEXT NOT NULL,
        total_quantity INTEGER NOT NULL CHECK(total_quantity >= 0)
    );
    CREATE TABLE IF NOT EXISTS loans (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        equipment_id INTEGER NOT NULL,
        student_roll TEXT NOT NULL,
        student_name TEXT NOT NULL,
        department TEXT NOT NULL,
        borrow_date TEXT NOT NULL,
        due_date TEXT NOT NULL,
        return_date TEXT,
        FOREIGN KEY(equipment_id) REFERENCES equipment(id) ON DELETE RESTRICT
    );
    CREATE INDEX IF NOT EXISTS idx_loans_equipment_id ON loans(equipment_id);
    CREATE INDEX IF NOT EXISTS idx_loans_student_roll ON loans(student_roll);
    CREATE INDEX IF NOT EXISTS idx_loans_return_date ON loans(return_date);
    """)

    # Seed if empty
    cur.execute("SELECT COUNT(*) FROM equipment;")
    if cur.fetchone()[0] == 0:
        samples = [
            ("Arduino Uno R3 Kit", "Embedded Systems", 5),
            ("Digital Storage Oscilloscope", "Electronics", 2),
            ("Raspberry Pi 4 Model B (4GB)", "IoT & Robotics", 4),
            ("Digital Multimeter (True-RMS)", "Test & Measurement", 6),
            ("FPGA Spartan-7 Board", "VLSI Design", 2)
        ]
        cur.executemany("INSERT INTO equipment (name, category, total_quantity) VALUES (?, ?, ?);", samples)
        conn.commit()
    conn.close()

def print_header(title):
    print("\n========================================================================")
    print(f"  {title}")
    print("========================================================================")

def print_divider():
    print("------------------------------------------------------------------------")

def show_equipment(db_path):
    print_header("LAB INVENTORY & AVAILABILITY STATUS")
    conn = sqlite3.connect(db_path)
    cur = conn.cursor()
    cur.execute("""
        SELECT e.id, e.name, e.category, e.total_quantity,
               COALESCE(COUNT(l.id), 0) AS borrowed_count
        FROM equipment e
        LEFT JOIN loans l ON e.id = l.equipment_id AND l.return_date IS NULL
        GROUP BY e.id
        ORDER BY e.id ASC;
    """)
    rows = cur.fetchall()
    conn.close()

    if not rows:
        print("  No equipment records found in the lab database.")
        return

    print(f"{'ID':<6}{'Equipment Name':<32}{'Category':<20}{'Total':<8}{'Borrowed':<10}{'Available':<10}")
    print_divider()
    for row in rows:
        eid, name, cat, total, borrowed = row
        avail = max(0, total - borrowed)
        disp_name = (name[:27] + "...") if len(name) > 30 else name
        disp_cat = (cat[:15] + "...") if len(cat) > 18 else cat
        print(f"{eid:<6}{disp_name:<32}{disp_cat:<20}{total:<8}{borrowed:<10}{avail:<10}")
    print_divider()

def add_equipment(db_path):
    print_header("ADD NEW LAB EQUIPMENT")
    name = input("  Enter Equipment Name : ").strip()
    if not name:
        print("  [x] Equipment name cannot be empty.")
        return

    cat = input("  Enter Category       : ").strip() or "General"
    qty_str = input("  Enter Total Quantity : ").strip()
    try:
        qty = int(qty_str)
        if qty <= 0:
            raise ValueError
    except ValueError:
        print("  [x] Invalid quantity. Must be a positive integer.")
        return

    try:
        conn = sqlite3.connect(db_path)
        cur = conn.cursor()
        cur.execute("INSERT INTO equipment (name, category, total_quantity) VALUES (?, ?, ?);", (name, cat, qty))
        new_id = cur.lastrowid
        conn.commit()
        conn.close()
        print(f"\n  [✓] Success: Added '{name}' (ID: #{new_id}) with stock of {qty} units.")
    except sqlite3.IntegrityError as e:
        print(f"\n  [x] Database Error: Equipment name '{name}' already exists or violated constraint.")

def issue_equipment(db_path):
    print_header("ISSUE EQUIPMENT TO STUDENT")
    show_equipment(db_path)

    eq_str = input("\n  Enter Equipment ID to Borrow: ").strip()
    try:
        eq_id = int(eq_str)
    except ValueError:
        print("  [x] Invalid ID.")
        return

    conn = sqlite3.connect(db_path)
    cur = conn.cursor()
    cur.execute("""
        SELECT e.name, e.total_quantity, COALESCE(COUNT(l.id), 0)
        FROM equipment e
        LEFT JOIN loans l ON e.id = l.equipment_id AND l.return_date IS NULL
        WHERE e.id = ?
        GROUP BY e.id;
    """, (eq_id,))
    row = cur.fetchone()

    if not row:
        print(f"  [x] Error: Equipment ID #{eq_id} does not exist.")
        conn.close()
        return

    eq_name, total, borrowed = row
    avail = max(0, total - borrowed)

    if avail <= 0:
        print(f"  [!] Cannot issue: All {total} units of '{eq_name}' are currently borrowed!")
        conn.close()
        return

    print(f"  Selected: {eq_name} (Available: {avail})")
    roll = input("  Student Roll / ID Number : ").strip()
    if not roll:
        print("  [x] Student Roll Number cannot be empty.")
        conn.close()
        return

    sname = input("  Student Full Name        : ").strip() or f"Student {roll}"
    dept = input("  Department / Class       : ").strip() or "ECE / CSE"

    today = datetime.date.today().isoformat()
    print(f"  Today's Date: {today}")
    due = input("  Due Date (YYYY-MM-DD)    : ").strip() or today

    cur.execute("""
        INSERT INTO loans (equipment_id, student_roll, student_name, department, borrow_date, due_date)
        VALUES (?, ?, ?, ?, ?, ?);
    """, (eq_id, roll, sname, dept, today, due))
    loan_id = cur.lastrowid
    conn.commit()
    conn.close()

    print("\n  [✓] LOAN CONFIRMED!")
    print(f"      Loan ID      : #{loan_id}")
    print(f"      Equipment    : {eq_name}")
    print(f"      Student      : {sname} ({roll})")
    print(f"      Department   : {dept}")
    print(f"      Borrow Date  : {today}")
    print(f"      Due Date     : {due}")
    print(f"      Remaining    : {avail - 1} unit(s) in lab.")

def return_equipment(db_path):
    print_header("RETURN BORROWED EQUIPMENT")
    loan_str = input("  Enter Loan ID to Return: ").strip()
    try:
        loan_id = int(loan_str)
    except ValueError:
        print("  [x] Invalid Loan ID.")
        return

    conn = sqlite3.connect(db_path)
    cur = conn.cursor()
    cur.execute("""
        SELECT l.id, l.return_date, l.student_roll, e.name
        FROM loans l
        JOIN equipment e ON l.equipment_id = e.id
        WHERE l.id = ?;
    """, (loan_id,))
    row = cur.fetchone()

    if not row:
        print(f"\n  [x] Return Failed: No loan found with ID #{loan_id}.")
        conn.close()
        return

    _, return_date, roll, eq_name = row
    if return_date is not None:
        print(f"\n  [!] ALREADY RETURNED: Loan #{loan_id} was already returned on {return_date}.")
        conn.close()
        return

    today = datetime.date.today().isoformat()
    cur.execute("UPDATE loans SET return_date = ? WHERE id = ?;", (today, loan_id))
    conn.commit()
    conn.close()

    print("\n  [✓] RETURN COMPLETED!")
    print(f"      Successfully returned '{eq_name}' (Student: {roll}).")
    print(f"      Date of Return: {today}")
    print("      Equipment stock has been restored to inventory.")

def view_active_loans(db_path):
    print_header("CURRENT ACTIVE & OVERDUE LOANS")
    conn = sqlite3.connect(db_path)
    cur = conn.cursor()
    cur.execute("""
        SELECT l.id, l.student_roll, l.student_name, e.name, l.due_date
        FROM loans l
        JOIN equipment e ON l.equipment_id = e.id
        WHERE l.return_date IS NULL
        ORDER BY l.id ASC;
    """)
    rows = cur.fetchall()
    conn.close()

    if not rows:
        print("  No active loans. All equipment is safely stored in the lab!")
        return

    today = datetime.date.today()
    print(f"{'Loan':<6}{'Roll No':<12}{'Student':<18}{'Equipment':<24}{'Due Date':<12}{'Status':<14}")
    print_divider()
    overdue_count = 0
    for row in rows:
        lid, roll, sname, eq_name, due_str = row
        due_date = datetime.date.fromisoformat(due_str)
        if due_date < today:
            diff = (today - due_date).days
            status = f"OVERDUE ({diff}d)"
            overdue_count += 1
        else:
            status = "ACTIVE"
        disp_student = (sname[:14] + "..") if len(sname) > 16 else sname
        disp_eq = (eq_name[:20] + "..") if len(eq_name) > 22 else eq_name
        print(f"#{lid:<5}{roll:<12}{disp_student:<18}{disp_eq:<24}{due_str:<12}{status:<14}")
    print_divider()
    print(f"  Total Active Loans: {len(rows)} | Overdue: {overdue_count}")

def view_statistics(db_path):
    print_header("LAB AUDIT & STATISTICAL SUMMARY")
    conn = sqlite3.connect(db_path)
    cur = conn.cursor()
    cur.execute("SELECT COUNT(*), COALESCE(SUM(total_quantity), 0) FROM equipment;")
    total_types, total_units = cur.fetchone()

    cur.execute("SELECT COUNT(*) FROM loans WHERE return_date IS NULL;")
    active_loans = cur.fetchone()[0]

    cur.execute("SELECT COUNT(*) FROM loans WHERE return_date IS NOT NULL;")
    returned_loans = cur.fetchone()[0]

    today = datetime.date.today().isoformat()
    cur.execute("SELECT COUNT(*) FROM loans WHERE return_date IS NULL AND due_date < ?;", (today,))
    overdue_loans = cur.fetchone()[0]
    conn.close()

    avail_units = max(0, total_units - active_loans)

    print(f"  • Unique Equipment Catalog Types : {total_types}")
    print(f"  • Total Physical Units in Lab   : {total_units}")
    print(f"  • Units Currently Available     : {avail_units}")
    print(f"  • Units Currently on Loan       : {active_loans}")
    print(f"  • Active Student Loans          : {active_loans}")
    print(f"  • Overdue Loans (Action Req.)   : {overdue_loans}")
    print(f"  • Total Completed Returns       : {returned_loans}")
    print_divider()

def main():
    db_path = DB_PATH
    if len(sys.argv) > 2 and sys.argv[1] == "--db":
        db_path = sys.argv[2]

    print("\n========================================================================")
    print("    CAMPUS LAB EQUIPMENT BORROWING SYSTEM")
    print("========================================================================")
    print(f"  Connecting to Lab Database: {db_path} ...")
    init_db(db_path)
    print("  [✓] Database Connected & Schema Verified Successfully!\n")

    while True:
        print("\n======================== MAIN MENU ========================")
        print("  1. Add Equipment to Inventory")
        print("  2. View Equipment & Stock Availability")
        print("  3. Issue Equipment to Student (Borrow)")
        print("  4. Return Equipment")
        print("  5. View Active & Overdue Loans")
        print("  6. Lab Statistics & Audit Summary")
        print("  7. Exit System")
        print("===========================================================")

        try:
            choice = input("  Select an option [1-7]: ").strip()
        except (EOFError, KeyboardInterrupt):
            print("\nExiting.")
            break

        if choice == "1":
            add_equipment(db_path)
        elif choice == "2":
            show_equipment(db_path)
        elif choice == "3":
            issue_equipment(db_path)
        elif choice == "4":
            return_equipment(db_path)
        elif choice == "5":
            view_active_loans(db_path)
        elif choice == "6":
            view_statistics(db_path)
        elif choice == "7":
            print("\n  Closing database connection safely. Goodbye!\n")
            break
        else:
            print("  [!] Please select a number from 1 to 7.")

if __name__ == "__main__":
    main()
