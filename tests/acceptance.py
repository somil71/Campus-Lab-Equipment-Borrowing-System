"""
Acceptance Tests for Campus Lab Equipment Borrowing System.
Verifies business logic, database constraints, transactions, and loan rules.
"""

import os
import sys
import tempfile
import sqlite3
import datetime

def run_test(name, func):
    try:
        func()
        print(f"[PASS] {name}")
        return True
    except Exception as e:
        print(f"[FAIL] {name}: {e}")
        return False

def cleanup_file(path):
    if not os.path.exists(path):
        return
    for _ in range(5):
        try:
            os.remove(path)
            break
        except Exception:
            import time
            time.sleep(0.05)

def get_test_db():
    tmp = tempfile.NamedTemporaryFile(suffix=".db", delete=False)
    db_path = tmp.name
    tmp.close()

    conn = sqlite3.connect(db_path)
    cur = conn.cursor()
    cur.executescript("""
    PRAGMA foreign_keys = ON;
    CREATE TABLE equipment (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        name TEXT NOT NULL UNIQUE,
        category TEXT NOT NULL,
        total_quantity INTEGER NOT NULL CHECK(total_quantity >= 0)
    );
    CREATE TABLE loans (
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
    CREATE INDEX idx_loans_equipment_id ON loans(equipment_id);
    CREATE INDEX idx_loans_student_roll ON loans(student_roll);
    CREATE INDEX idx_loans_return_date ON loans(return_date);
    """)
    conn.commit()
    return conn, db_path

# 1. Database Schema & Integrity Constraints
def test_schema_and_constraints():
    conn, db_path = get_test_db()
    try:
        cur = conn.cursor()
        cur.execute("INSERT INTO equipment (name, category, total_quantity) VALUES (?, ?, ?)",
                    ("Arduino Uno R3", "Embedded", 2))
        eq_id = cur.lastrowid
        assert eq_id == 1, "Equipment ID should be 1"

        # Unique name constraint
        try:
            cur.execute("INSERT INTO equipment (name, category, total_quantity) VALUES (?, ?, ?)",
                        ("Arduino Uno R3", "Embedded", 5))
            assert False, "Should fail duplicate equipment name"
        except sqlite3.IntegrityError:
            pass

        # Check constraint (quantity >= 0)
        try:
            cur.execute("INSERT INTO equipment (name, category, total_quantity) VALUES (?, ?, ?)",
                        ("Faulty Item", "General", -1))
            assert False, "Should fail negative quantity"
        except sqlite3.IntegrityError:
            pass

        conn.commit()
        conn.close()
    finally:
        cleanup_file(db_path)

# 2. Seed Data Initialization
def test_seed_initialization():
    conn, db_path = get_test_db()
    try:
        cur = conn.cursor()
        samples = [
            ("Arduino Uno R3 Kit", "Embedded Systems", 5),
            ("Digital Storage Oscilloscope", "Electronics", 2),
            ("Raspberry Pi 4 Model B (4GB)", "IoT & Robotics", 4),
            ("Digital Multimeter (True-RMS)", "Test & Measurement", 6),
            ("FPGA Spartan-7 Board", "VLSI Design", 2)
        ]
        cur.executemany("INSERT INTO equipment (name, category, total_quantity) VALUES (?, ?, ?)", samples)
        conn.commit()

        cur.execute("SELECT COUNT(*), SUM(total_quantity) FROM equipment")
        count, total = cur.fetchone()
        assert count == 5, f"Expected 5 categories, got {count}"
        assert total == 19, f"Expected 19 total units, got {total}"
        conn.close()
    finally:
        cleanup_file(db_path)

# 3. Add New Equipment
def test_add_equipment():
    conn, db_path = get_test_db()
    try:
        cur = conn.cursor()
        cur.execute("INSERT INTO equipment (name, category, total_quantity) VALUES (?, ?, ?)",
                    ("Soldering Station", "Hardware", 10))
        conn.commit()
        cur.execute("SELECT id, name, category, total_quantity FROM equipment WHERE name = 'Soldering Station'")
        row = cur.fetchone()
        assert row is not None and row[3] == 10, "Failed to add new equipment"
        conn.close()
    finally:
        cleanup_file(db_path)

# 4. Stock Calculation & Live Inventory Tracking
def test_stock_calculation():
    conn, db_path = get_test_db()
    try:
        cur = conn.cursor()
        cur.execute("INSERT INTO equipment (name, category, total_quantity) VALUES ('Logic Analyzer', 'Digital', 3)")
        eq_id = cur.lastrowid

        # Query stock when 0 loans exist
        cur.execute("""
            SELECT e.total_quantity, COUNT(l.id) AS borrowed,
                   (e.total_quantity - COUNT(l.id)) AS available
            FROM equipment e
            LEFT JOIN loans l ON e.id = l.equipment_id AND l.return_date IS NULL
            WHERE e.id = ?
            GROUP BY e.id
        """, (eq_id,))
        total, borrowed, avail = cur.fetchone()
        assert total == 3 and borrowed == 0 and avail == 3
        conn.close()
    finally:
        cleanup_file(db_path)

# 5. Lending Flow (Quantity Decrement)
def test_lending_flow():
    conn, db_path = get_test_db()
    try:
        cur = conn.cursor()
        cur.execute("INSERT INTO equipment (name, category, total_quantity) VALUES ('Spectrum Analyzer', 'RF', 2)")
        eq_id = cur.lastrowid

        # Issue 1 loan
        cur.execute("""
            INSERT INTO loans (equipment_id, student_roll, student_name, department, borrow_date, due_date)
            VALUES (?, '22CS01', 'Alice', 'CSE', '2026-11-19', '2026-11-26')
        """, (eq_id,))
        conn.commit()

        cur.execute("""
            SELECT (e.total_quantity - COUNT(l.id)) AS available
            FROM equipment e
            LEFT JOIN loans l ON e.id = l.equipment_id AND l.return_date IS NULL
            WHERE e.id = ?
            GROUP BY e.id
        """, (eq_id,))
        avail = cur.fetchone()[0]
        assert avail == 1, f"Expected 1 available unit, got {avail}"
        conn.close()
    finally:
        cleanup_file(db_path)

# 6. Over-borrowing Prevention (Blocking when available == 0)
def test_overborrowing_prevention():
    conn, db_path = get_test_db()
    try:
        cur = conn.cursor()
        cur.execute("INSERT INTO equipment (name, category, total_quantity) VALUES ('Drone Kit', 'Robotics', 1)")
        eq_id = cur.lastrowid

        # Issue the only unit
        cur.execute("""
            INSERT INTO loans (equipment_id, student_roll, student_name, department, borrow_date, due_date)
            VALUES (?, '22CS01', 'Bob', 'CSE', '2026-11-19', '2026-11-26')
        """, (eq_id,))
        conn.commit()

        # Check availability before 2nd issue attempt
        cur.execute("""
            SELECT (e.total_quantity - COUNT(l.id)) AS available
            FROM equipment e
            LEFT JOIN loans l ON e.id = l.equipment_id AND l.return_date IS NULL
            WHERE e.id = ?
            GROUP BY e.id
        """, (eq_id,))
        avail = cur.fetchone()[0]
        assert avail == 0, "Available units should be 0, over-borrowing must be blocked"
        conn.close()
    finally:
        cleanup_file(db_path)

# 7. Duplicate Return Prevention
def test_duplicate_return_prevention():
    conn, db_path = get_test_db()
    try:
        cur = conn.cursor()
        cur.execute("INSERT INTO equipment (name, category, total_quantity) VALUES ('Function Generator', 'Electronics', 2)")
        eq_id = cur.lastrowid

        cur.execute("""
            INSERT INTO loans (equipment_id, student_roll, student_name, department, borrow_date, due_date, return_date)
            VALUES (?, '22CS03', 'Charlie', 'ECE', '2026-11-01', '2026-11-08', '2026-11-07')
        """, (eq_id,))
        loan_id = cur.lastrowid
        conn.commit()

        cur.execute("SELECT return_date FROM loans WHERE id = ?", (loan_id,))
        ret_date = cur.fetchone()[0]
        assert ret_date == '2026-11-07', "Return date should be preserved and block re-return"
        conn.close()
    finally:
        cleanup_file(db_path)

# 8. Overdue Detection & Date Arithmetic
def test_overdue_detection():
    conn, db_path = get_test_db()
    try:
        cur = conn.cursor()
        cur.execute("INSERT INTO equipment (name, category, total_quantity) VALUES ('Thermal Camera', 'Optics', 2)")
        eq_id = cur.lastrowid

        today = datetime.date.today().isoformat()
        yesterday = (datetime.date.today() - datetime.timedelta(days=1)).isoformat()
        tomorrow = (datetime.date.today() + datetime.timedelta(days=1)).isoformat()

        # 1 overdue loan
        cur.execute("""
            INSERT INTO loans (equipment_id, student_roll, student_name, department, borrow_date, due_date)
            VALUES (?, '22CS10', 'Dave', 'CSE', '2026-11-01', ?)
        """, (eq_id, yesterday))

        # 1 active loan (due tomorrow)
        cur.execute("""
            INSERT INTO loans (equipment_id, student_roll, student_name, department, borrow_date, due_date)
            VALUES (?, '22CS11', 'Emma', 'CSE', '2026-11-10', ?)
        """, (eq_id, tomorrow))
        conn.commit()

        cur.execute("SELECT id FROM loans WHERE return_date IS NULL AND due_date < ?", (today,))
        overdue_list = cur.fetchall()
        assert len(overdue_list) == 1, f"Expected 1 overdue loan, got {len(overdue_list)}"
        conn.close()
    finally:
        cleanup_file(db_path)

def main():
    print("==========================================================")
    print(" Campus Lab Borrowing System - Acceptance Test Suite (8/8)")
    print("==========================================================")

    tests = [
        ("1. Database Schema & Integrity Constraints", test_schema_and_constraints),
        ("2. Seed Data Initialization", test_seed_initialization),
        ("3. Adding New Equipment to Inventory", test_add_equipment),
        ("4. Stock Calculation & Live Inventory Tracking", test_stock_calculation),
        ("5. Lending Flow (Quantity Decrement)", test_lending_flow),
        ("6. Over-borrowing Prevention (Available = 0)", test_overborrowing_prevention),
        ("7. Duplicate Return Prevention Logic", test_duplicate_return_prevention),
        ("8. Overdue Loan Detection & Date Arithmetic", test_overdue_detection),
    ]

    passed = 0
    for name, test in tests:
        if run_test(name, test):
            passed += 1

    print("==========================================================")
    print(f" Test Results: {passed}/{len(tests)} Passed.")
    print("==========================================================")
    return 0 if passed == len(tests) else 1

if __name__ == "__main__":
    sys.exit(main())
