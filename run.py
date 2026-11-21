#!/usr/bin/env python3
"""
Campus Lab Equipment Borrowing System
Terminal Runner for Antigravity IDE Terminal.
Operates on the exact same 5 relational tables:
labs.txt, students.txt, equipment.txt, loans.txt, fines.txt
"""

import os
import sys
import datetime

LABS_FILE = "labs.txt"
STUDENTS_FILE = "students.txt"
EQUIPMENT_FILE = "equipment.txt"
LOANS_FILE = "loans.txt"
FINES_FILE = "fines.txt"

def print_header(title):
    print("\n========================================================================")
    print(f"  {title}")
    print("========================================================================")

def print_divider():
    print("------------------------------------------------------------------------")

def init_db():
    if not os.path.exists(LABS_FILE) or os.path.getsize(LABS_FILE) == 0:
        with open(LABS_FILE, "w", encoding="utf-8") as f:
            f.write("LAB-IOT|IoT & Robotics Lab|Block B - Room 304|Prof. S. K. Verma\n")
            f.write("LAB-ECE|Advanced Electronics Lab|Block A - Room 102|Dr. Anita Rao\n")
            f.write("LAB-VLSI|Embedded & VLSI Design Lab|Block B - Room 308|Dr. R. K. Sharma\n")

    if not os.path.exists(STUDENTS_FILE) or os.path.getsize(STUDENTS_FILE) == 0:
        with open(STUDENTS_FILE, "w", encoding="utf-8") as f:
            f.write("23CS101|Aarav Sharma|CSE|aarav@campus.edu|2\n")
            f.write("23CS102|Bhavna Patel|CSE|bhavna@campus.edu|2\n")
            f.write("23EC045|Chirag Verma|ECE|chirag@campus.edu|2\n")
            f.write("23EC078|Divya Nair|ECE|divya@campus.edu|2\n")

    if not os.path.exists(EQUIPMENT_FILE) or os.path.getsize(EQUIPMENT_FILE) == 0:
        with open(EQUIPMENT_FILE, "w", encoding="utf-8") as f:
            f.write("101|Arduino Uno R3 Kit|Microcontrollers|LAB-IOT|5|1|GOOD\n")
            f.write("102|Digital Storage Oscilloscope|Test & Measurement|LAB-ECE|2|0|GOOD\n")
            f.write("103|Raspberry Pi 4 Model B (4GB)|Single Board Computers|LAB-IOT|4|0|GOOD\n")
            f.write("104|Digital Multimeter (True-RMS)|Instruments|LAB-ECE|6|0|GOOD\n")
            f.write("105|FPGA Spartan-7 Board|VLSI Boards|LAB-VLSI|2|0|GOOD\n")

    for fpath in (LOANS_FILE, FINES_FILE):
        if not os.path.exists(fpath):
            with open(fpath, "w", encoding="utf-8") as f:
                pass

def read_lines(filepath):
    if not os.path.exists(filepath):
        return []
    with open(filepath, "r", encoding="utf-8") as f:
        return [line.strip() for line in f if line.strip()]

def write_lines(filepath, lines):
    with open(filepath, "w", encoding="utf-8") as f:
        for line in lines:
            f.write(line + "\n")

# Operations
def view_labs():
    print_header("CAMPUS LABS & PHYSICAL ROOM LOCATIONS")
    lines = read_lines(LABS_FILE)
    print(f"{'Lab ID':<12}{'Lab Name':<30}{'Location/Room':<22}{'Faculty In-Charge':<20}")
    print_divider()
    for line in lines:
        lid, name, room, fac = line.split("|")
        print(f"{lid:<12}{name:<30}{room:<22}{fac:<20}")
    print_divider()

def view_equipment():
    print_header("LAB HARDWARE INVENTORY (WITH ROOM LOCATIONS)")
    lines = read_lines(EQUIPMENT_FILE)
    print(f"{'ID':<6}{'Equipment Name':<28}{'Category':<16}{'Lab ID':<10}{'Total':<7}{'Avail':<7}{'Condition':<12}")
    print_divider()
    for line in lines:
        parts = line.split("|")
        eid, name, cat, lab, tot, bor, cond = parts[0], parts[1], parts[2], parts[3], int(parts[4]), int(parts[5]), parts[6]
        avail = max(0, tot - bor)
        disp_name = (name[:24] + "..") if len(name) > 26 else name
        disp_cat = (cat[:12] + "..") if len(cat) > 14 else cat
        print(f"{eid:<6}{disp_name:<28}{disp_cat:<16}{lab:<10}{tot:<7}{avail:<7}{cond:<12}")
    print_divider()

def count_active_loans(roll):
    lines = read_lines(LOANS_FILE)
    count = 0
    for line in lines:
        parts = line.split("|")
        if parts[2] == roll and parts[5] == "PENDING":
            count += 1
    return count

def view_students():
    print_header("REGISTERED STUDENTS & BORROW QUOTAS")
    lines = read_lines(STUDENTS_FILE)
    print(f"{'Roll No':<12}{'Student Name':<20}{'Dept':<8}{'Contact Email':<22}{'Active':<10}{'Limit':<8}")
    print_divider()
    for line in lines:
        roll, name, dept, email, limit = line.split("|")
        active = count_active_loans(roll)
        print(f"{roll:<12}{name:<20}{dept:<8}{email:<22}{str(active) + ' units':<10}{limit:<8}")
    print_divider()

def register_student():
    print_header("REGISTER NEW STUDENT")
    roll = input("  Enter University Roll Number : ").strip()
    if not roll:
        print("  [!] Roll number cannot be empty.")
        return
    lines = read_lines(STUDENTS_FILE)
    for l in lines:
        if l.split("|")[0].lower() == roll.lower():
            print(f"  [!] Student with roll '{roll}' is already registered.")
            return

    name = input("  Enter Student Full Name      : ").strip() or f"Student {roll}"
    dept = input("  Enter Department (CSE/ECE)   : ").strip() or "CSE"
    email = input("  Enter Contact Email          : ").strip() or f"{roll.lower()}@campus.edu"

    lines.append(f"{roll}|{name}|{dept}|{email}|2")
    write_lines(STUDENTS_FILE, lines)
    print(f"\n  [✓] SUCCESS: Student '{name}' ({roll}) registered! (Quota: 2 units)")

def add_equipment():
    print_header("ADD NEW LAB HARDWARE EQUIPMENT")
    name = input("  Enter Equipment Name : ").strip()
    if not name:
        print("  [!] Name cannot be empty.")
        return
    cat = input("  Enter Category       : ").strip() or "Sensors & Boards"
    view_labs()
    lab = input("  Enter Lab ID to Assign Item (e.g., LAB-IOT, LAB-ECE) : ").strip() or "LAB-IOT"
    qty_str = input("  Enter Quantity       : ").strip()
    try:
        qty = int(qty_str)
        if qty <= 0: raise ValueError
    except ValueError:
        print("  [!] Invalid quantity.")
        return

    lines = read_lines(EQUIPMENT_FILE)
    new_id = 101 if not lines else int(lines[-1].split("|")[0]) + 1
    lines.append(f"{new_id}|{name}|{cat}|{lab}|{qty}|0|GOOD")
    write_lines(EQUIPMENT_FILE, lines)
    print(f"\n  [✓] SUCCESS: Added '{name}' (ID: #{new_id}) with {qty} units in {lab}.")

def issue_equipment():
    print_header("ISSUE HARDWARE TO STUDENT (QUOTA PROTECTED)")
    view_equipment()

    eq_str = input("\n  Enter Equipment ID to Borrow : ").strip()
    try:
        eq_id = int(eq_str)
    except ValueError:
        print("  [!] Invalid ID.")
        return

    roll = input("  Enter Student Roll Number    : ").strip()
    if not roll:
        print("  [!] Roll number cannot be empty.")
        return

    # Check student
    std_lines = read_lines(STUDENTS_FILE)
    student = None
    for l in std_lines:
        if l.split("|")[0].lower() == roll.lower():
            student = l.split("|")
            break

    if not student:
        print(f"\n  [x] REJECTED: Student '{roll}' is not registered in the system!")
        print("      Please register the student first (Menu Option 3).")
        return

    limit = int(student[4])
    active = count_active_loans(student[0])
    if active >= limit:
        print(f"\n  [x] REJECTED (QUOTA EXCEEDED): Student '{student[0]}' has already borrowed their limit (Max {limit} units)!")
        print("      Student must return an existing item before borrowing new hardware.")
        return

    # Check equipment
    eq_lines = read_lines(EQUIPMENT_FILE)
    eq_idx = -1
    for i, l in enumerate(eq_lines):
        if int(l.split("|")[0]) == eq_id:
            eq_idx = i
            break

    if eq_idx == -1:
        print(f"\n  [x] Error: Equipment ID #{eq_id} does not exist.")
        return

    parts = eq_lines[eq_idx].split("|")
    tot, bor, cond = int(parts[4]), int(parts[5]), parts[6]
    avail = tot - bor

    if cond != "GOOD":
        print("\n  [!] HARDWARE BLOCKED: This equipment is currently marked under MAINTENANCE.")
        return

    if avail <= 0:
        print("\n  [!] OUT OF STOCK: All units of this hardware are currently on loan!")
        return

    today = datetime.date.today().isoformat()
    print(f"  Today's Issue Date           : {today}")
    due = input("  Enter Due Date (YYYY-MM-DD)  [Press Enter for +7 days]: ").strip()
    if not due:
        due = (datetime.date.today() + datetime.timedelta(days=7)).isoformat()

    # Update equipment
    parts[5] = str(bor + 1)
    eq_lines[eq_idx] = "|".join(parts)
    write_lines(EQUIPMENT_FILE, eq_lines)

    # Insert loan
    loan_lines = read_lines(LOANS_FILE)
    new_loan_id = 1001 if not loan_lines else int(loan_lines[-1].split("|")[0]) + 1
    loan_lines.append(f"{new_loan_id}|{eq_id}|{student[0]}|{today}|{due}|PENDING|ACTIVE")
    write_lines(LOANS_FILE, loan_lines)

    print("\n  [✓] LOAN CONFIRMED!")
    print(f"      Loan Receipt ID : #{new_loan_id}")
    print(f"      Student Roll    : {student[0]}")
    print(f"      Due Date        : {due}")
    print("      Status          : ACTIVE (Transaction saved to disk)")

def return_equipment():
    print_header("RETURN HARDWARE (WITH AUTOMATED FINE CALCULATOR)")
    l_str = input("  Enter Loan Receipt ID to Return: ").strip()
    try:
        loan_id = int(l_str)
    except ValueError:
        print("  [!] Invalid Loan ID.")
        return

    loan_lines = read_lines(LOANS_FILE)
    l_idx = -1
    for i, l in enumerate(loan_lines):
        if int(l.split("|")[0]) == loan_id:
            l_idx = i
            break

    if l_idx == -1:
        print(f"\n  [x] Error: Loan ID #{loan_id} not found in database.")
        return

    parts = loan_lines[l_idx].split("|")
    if parts[5] != "PENDING":
        print(f"\n  [!] DUPLICATE RETURN ATTEMPT: Loan #{loan_id} was already returned earlier!")
        return

    eq_id = int(parts[1])
    roll = parts[2]
    due_date = datetime.date.fromisoformat(parts[4])
    today = datetime.date.today()
    today_str = today.isoformat()

    # Update loan
    parts[5] = today_str
    parts[6] = "RETURNED"
    loan_lines[l_idx] = "|".join(parts)
    write_lines(LOANS_FILE, loan_lines)

    # Restore equipment stock
    eq_lines = read_lines(EQUIPMENT_FILE)
    eq_name = "Hardware"
    for i, el in enumerate(eq_lines):
        ep = el.split("|")
        if int(ep[0]) == eq_id:
            eq_name = ep[1]
            ep[5] = str(max(0, int(ep[5]) - 1))
            eq_lines[i] = "|".join(ep)
            break
    write_lines(EQUIPMENT_FILE, eq_lines)

    # Calculate fine
    if today > due_date:
        overdue_days = (today - due_date).days
        fine_amt = overdue_days * 10
        fine_lines = read_lines(FINES_FILE)
        new_fid = 501 if not fine_lines else int(fine_lines[-1].split("|")[0]) + 1
        fine_lines.append(f"{new_fid}|{loan_id}|{roll}|{overdue_days}|{fine_amt}|PENDING")
        write_lines(FINES_FILE, fine_lines)

        print("\n  [!] RETURN ACCEPTED WITH OVERDUE PENALTY!")
        print(f"      Equipment    : {eq_name}")
        print(f"      Return Date  : {today_str}")
        print(f"      Days Overdue : {overdue_days} day(s) late")
        print(f"      Late Fine    : ₹{fine_amt} (Rate: ₹10/day)")
        print("      Status       : PENDING PAYMENT (Recorded in fines table)")
    else:
        print("\n  [✓] RETURN SUCCESSFUL!")
        print(f"      Equipment   : {eq_name}")
        print(f"      Return Date : {today_str} (On-Time)")
        print("      Late Fine   : ₹0.00 (No penalty)")
        print("      Stock restored to inventory automatically.")

def view_active_loans():
    print_header("ACTIVE & OVERDUE HARDWARE LOANS")
    lines = read_lines(LOANS_FILE)
    active = [l.split("|") for l in lines if l.split("|")[5] == "PENDING"]

    if not active:
        print("  No active loans. All lab hardware is currently in stock.")
        return

    today = datetime.date.today()
    print(f"{'Loan ID':<8}{'Roll No':<12}{'EquipID':<8}{'Issue Date':<12}{'Due Date':<12}{'Status':<14}")
    print_divider()

    overdue_count = 0
    for p in active:
        lid, eid, roll, idate, ddate = p[0], p[1], p[2], p[3], p[4]
        due = datetime.date.fromisoformat(ddate)
        if today > due:
            diff = (today - due).days
            stat = f"OVERDUE ({diff}d)"
            overdue_count += 1
        else:
            stat = "ACTIVE"
        print(f"#{lid:<7}{roll:<12}{eid:<8}{idate:<12}{ddate:<12}{stat:<14}")
    print_divider()
    print(f"  Total Active: {len(active)} | Overdue: {overdue_count}")

def view_fines():
    print_header("STUDENT LATE FINES & PENALTIES")
    lines = read_lines(FINES_FILE)
    if not lines:
        print("  No fines on record. All students have returned equipment on time!")
        return

    print(f"{'Fine ID':<8}{'Loan ID':<10}{'Roll No':<12}{'Overdue Days':<14}{'Amount (₹)':<12}{'Status':<12}")
    print_divider()
    for l in lines:
        fid, lid, roll, days, amt, stat = l.split("|")
        print(f"#{fid:<7}#{lid:<9}{roll:<12}{days + ' days':<14}{'₹' + amt:<12}{stat:<12}")
    print_divider()

    ans = input("  Do you want to mark a fine as PAID? (y/n): ").strip().lower()
    if ans == "y":
        fid_str = input("  Enter Fine ID to pay: ").strip()
        updated = False
        for i, l in enumerate(lines):
            p = l.split("|")
            if p[0] == fid_str:
                p[5] = "PAID"
                lines[i] = "|".join(p)
                updated = True
                break
        if updated:
            write_lines(FINES_FILE, lines)
            print(f"  [✓] Fine #{fid_str} marked as PAID!")
        else:
            print("  [x] Fine ID not found.")

def inspect_db_files():
    print_header("FACULTY AUDIT: RAW 3NF DATABASE FILES ON DISK")
    tables = [
        ("1. LABS", LABS_FILE),
        ("2. STUDENTS", STUDENTS_FILE),
        ("3. EQUIPMENT", EQUIPMENT_FILE),
        ("4. LOANS", LOANS_FILE),
        ("5. FINES", FINES_FILE)
    ]
    for name, path in tables:
        print(f"\n--> Table: {name} ({path})")
        lines = read_lines(path)
        if not lines:
            print("    (Empty)")
        else:
            for l in lines:
                print(f"    {l}")
    print_divider()
    print("  All 5 tables are stored as relational text files with primary/foreign keys.")
    print("  You can also inspect or edit them using Notepad or Excel!")

def main():
    init_db()
    print("\n========================================================================")
    print("   CAMPUS LAB EQUIPMENT BORROWING SYSTEM (C++ OOP & RELATIONAL DB)     ")
    print("========================================================================")
    print("  [✓] Relational Database initialized (5 Tables: Labs, Students, Equip, Loans, Fines)")

    while True:
        print("\n========================== MAIN MENU ==========================")
        print("  1. View Equipment Inventory & Live Stock")
        print("  2. View Lab Rooms & Physical Locations")
        print("  3. View / Register Students (Quota: 2 Units)")
        print("  4. Issue Hardware to Student (Borrow)")
        print("  5. Return Hardware (Automated Fine Calculator)")
        print("  6. View Active & Overdue Hardware Loans")
        print("  7. View Late Fines & Mark Payments")
        print("  8. Add New Equipment to a Lab")
        print("  9. Inspect All 5 Raw Database Files (Show Faculty)")
        print("  10. Exit System")
        print("===============================================================")

        try:
            choice = input("  Select option [1-10]: ").strip()
        except (EOFError, KeyboardInterrupt):
            print("\n  Closing system safely. Goodbye!\n")
            break

        if choice == "1": view_equipment()
        elif choice == "2": view_labs()
        elif choice == "3":
            view_students()
            ans = input("  Do you want to register a new student? (y/n): ").strip().lower()
            if ans == "y": register_student()
        elif choice == "4": issue_equipment()
        elif choice == "5": return_equipment()
        elif choice == "6": view_active_loans()
        elif choice == "7": view_fines()
        elif choice == "8": add_equipment()
        elif choice == "9": inspect_db_files()
        elif choice == "10":
            print("\n  All transactions saved permanently to disk. Goodbye!\n")
            break
        else:
            print("  [!] Please select a valid number from 1 to 10.")

if __name__ == "__main__":
    main()
