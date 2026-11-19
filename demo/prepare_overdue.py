#!/usr/bin/env python3
"""
Prepares an isolated demo environment with active, due today, and overdue loans.
"""

import os
import sys
import sqlite3
import datetime

def prepare_demo():
    demo_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), "demo_run"))
    os.makedirs(demo_dir, exist_ok=True)
    db_path = os.path.join(demo_dir, "lab_equipment.db")

    if os.path.exists(db_path):
        try:
            os.remove(db_path)
        except Exception:
            pass

    today = datetime.date.today()
    yesterday = (today - datetime.timedelta(days=2)).isoformat()
    today_str = today.isoformat()
    tomorrow = (today + datetime.timedelta(days=3)).isoformat()
    last_week = (today - datetime.timedelta(days=7)).isoformat()

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
    """)

    # Seed equipment
    equipment_items = [
        ("Arduino Uno R3 Kit", "Embedded Systems", 5),
        ("Digital Storage Oscilloscope", "Electronics", 2),
        ("Raspberry Pi 4 Model B (4GB)", "IoT & Robotics", 4),
        ("Digital Multimeter (True-RMS)", "Test & Measurement", 6),
        ("FPGA Spartan-7 Board", "VLSI Design", 2)
    ]
    cur.executemany("INSERT INTO equipment (name, category, total_quantity) VALUES (?, ?, ?);", equipment_items)

    # Seed loans: Overdue, Due Today, Due Tomorrow, Returned
    loans = [
        (1, "23EC012", "Aarav Sharma", "ECE", last_week, yesterday, None),       # Overdue
        (2, "23CS045", "Bhavna Patel", "CSE", last_week, today_str, None),        # Due Today
        (3, "23EE008", "Chirag Verma", "EE",  last_week, tomorrow, None),         # Due in 3 days
        (4, "23ME019", "Divya Nair",   "ME",  last_week, yesterday, today_str),   # Already returned
    ]
    cur.executemany("""
        INSERT INTO loans (equipment_id, student_roll, student_name, department, borrow_date, due_date, return_date)
        VALUES (?, ?, ?, ?, ?, ?, ?);
    """, loans)

    conn.commit()
    conn.close()

    root_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
    if sys.platform == "win32":
        exe_path = os.path.join(root_dir, "build", "campus_lab.exe")
    else:
        exe_path = os.path.join(root_dir, "build", "campus_lab")

    print("\n==============================================================")
    print(" [✓] DEMO ENVIRONMENT PREPARED SUCCESSFULLY!")
    print("==============================================================")
    print(f" Demo Directory : {demo_dir}")
    print(f" Demo Database  : {db_path}")
    print(f" Executable     : {exe_path}")
    print("\nTo demonstrate live to faculty:")
    print(f" 1. cd \"{demo_dir}\"")
    if sys.platform == "win32":
        print(f" 2. & \"{exe_path}\"")
    else:
        print(f" 2. \"{exe_path}\"")
    print(" 3. Choose Menu Option 5 to immediately show the active & overdue loans!")
    print("==============================================================\n")

if __name__ == "__main__":
    prepare_demo()
