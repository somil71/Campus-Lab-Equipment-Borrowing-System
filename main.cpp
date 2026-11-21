#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>
#include <ctime>
#include <cstdlib>
#include <algorithm>

using namespace std;

// ==========================================
// 1. DATA MODELS (OOP 5-ENTITY SCHEMA)
// ==========================================

// Entity 1: LAB
class Lab {
private:
    string labId;
    string labName;
    string roomNo;
    string inchargeFaculty;

public:
    Lab() {}
    Lab(string id, string name, string room, string faculty)
        : labId(id), labName(name), roomNo(room), inchargeFaculty(faculty) {}

    string getLabId() const { return labId; }
    string getLabName() const { return labName; }
    string getRoomNo() const { return roomNo; }
    string getInchargeFaculty() const { return inchargeFaculty; }

    string toRow() const {
        return labId + "|" + labName + "|" + roomNo + "|" + inchargeFaculty;
    }

    static Lab fromRow(const string& row) {
        stringstream ss(row);
        string id, name, room, fac;
        getline(ss, id, '|');
        getline(ss, name, '|');
        getline(ss, room, '|');
        getline(ss, fac, '|');
        return Lab(id, name, room, fac);
    }
};

// Entity 2: STUDENT
class Student {
private:
    string rollNo;
    string name;
    string department;
    string contact;
    int maxBorrowLimit;

public:
    Student() : maxBorrowLimit(2) {}
    Student(string roll, string name, string dept, string contact, int maxLimit = 2)
        : rollNo(roll), name(name), department(dept), contact(contact), maxBorrowLimit(maxLimit) {}

    string getRollNo() const { return rollNo; }
    string getName() const { return name; }
    string getDepartment() const { return department; }
    string getContact() const { return contact; }
    int getMaxBorrowLimit() const { return maxBorrowLimit; }

    string toRow() const {
        return rollNo + "|" + name + "|" + department + "|" + contact + "|" + to_string(maxBorrowLimit);
    }

    static Student fromRow(const string& row) {
        stringstream ss(row);
        string roll, name, dept, contact, limitStr;
        getline(ss, roll, '|');
        getline(ss, name, '|');
        getline(ss, dept, '|');
        getline(ss, contact, '|');
        getline(ss, limitStr, '|');
        int limit = limitStr.empty() ? 2 : stoi(limitStr);
        return Student(roll, name, dept, contact, limit);
    }
};

// Entity 3: EQUIPMENT
class Equipment {
private:
    int id;
    string name;
    string category;
    string labId; // Foreign Key referencing Lab
    int totalQuantity;
    int borrowedCount;
    string condition; // "GOOD", "MAINTENANCE"

public:
    Equipment() : id(0), totalQuantity(0), borrowedCount(0), condition("GOOD") {}
    Equipment(int id, string name, string category, string labId, int total, int borrowed = 0, string cond = "GOOD")
        : id(id), name(name), category(category), labId(labId),
          totalQuantity(total), borrowedCount(borrowed), condition(cond) {}

    int getId() const { return id; }
    string getName() const { return name; }
    string getCategory() const { return category; }
    string getLabId() const { return labId; }
    int getTotalQuantity() const { return totalQuantity; }
    int getBorrowedCount() const { return borrowedCount; }
    int getAvailableQuantity() const {
        int avail = totalQuantity - borrowedCount;
        return avail > 0 ? avail : 0;
    }
    string getCondition() const { return condition; }

    void incrementBorrowed() { borrowedCount++; }
    void decrementBorrowed() { if (borrowedCount > 0) borrowedCount--; }
    void setCondition(const string& cond) { condition = cond; }

    string toRow() const {
        return to_string(id) + "|" + name + "|" + category + "|" + labId + "|" +
               to_string(totalQuantity) + "|" + to_string(borrowedCount) + "|" + condition;
    }

    static Equipment fromRow(const string& row) {
        stringstream ss(row);
        string idStr, name, cat, lab, totStr, borStr, cond;
        getline(ss, idStr, '|');
        getline(ss, name, '|');
        getline(ss, cat, '|');
        getline(ss, lab, '|');
        getline(ss, totStr, '|');
        getline(ss, borStr, '|');
        getline(ss, cond, '|');

        int id = idStr.empty() ? 0 : stoi(idStr);
        int tot = totStr.empty() ? 0 : stoi(totStr);
        int bor = borStr.empty() ? 0 : stoi(borStr);
        if (cond.empty()) cond = "GOOD";

        return Equipment(id, name, cat, lab, tot, bor, cond);
    }
};

// Entity 4: LOAN
class Loan {
private:
    int loanId;
    int equipmentId;
    string studentRoll;
    string issueDate;
    string dueDate;
    string returnDate; // "PENDING" or date
    string status;     // "ACTIVE", "RETURNED", "OVERDUE"

public:
    Loan() : loanId(0), equipmentId(0), returnDate("PENDING"), status("ACTIVE") {}
    Loan(int lId, int eId, string roll, string iDate, string dDate, string rDate = "PENDING", string stat = "ACTIVE")
        : loanId(lId), equipmentId(eId), studentRoll(roll),
          issueDate(iDate), dueDate(dDate), returnDate(rDate), status(stat) {}

    int getLoanId() const { return loanId; }
    int getEquipmentId() const { return equipmentId; }
    string getStudentRoll() const { return studentRoll; }
    string getIssueDate() const { return issueDate; }
    string getDueDate() const { return dueDate; }
    string getReturnDate() const { return returnDate; }
    string getStatus() const { return status; }

    bool isReturned() const { return returnDate != "PENDING"; }
    void markReturned(const string& date) {
        returnDate = date;
        status = "RETURNED";
    }
    void setStatus(const string& stat) { status = stat; }

    string toRow() const {
        return to_string(loanId) + "|" + to_string(equipmentId) + "|" + studentRoll + "|" +
               issueDate + "|" + dueDate + "|" + returnDate + "|" + status;
    }

    static Loan fromRow(const string& row) {
        stringstream ss(row);
        string lIdStr, eIdStr, roll, iDate, dDate, rDate, stat;
        getline(ss, lIdStr, '|');
        getline(ss, eIdStr, '|');
        getline(ss, roll, '|');
        getline(ss, iDate, '|');
        getline(ss, dDate, '|');
        getline(ss, rDate, '|');
        getline(ss, stat, '|');

        int lId = lIdStr.empty() ? 0 : stoi(lIdStr);
        int eId = eIdStr.empty() ? 0 : stoi(eIdStr);
        if (rDate.empty()) rDate = "PENDING";
        if (stat.empty()) stat = "ACTIVE";

        return Loan(lId, eId, roll, iDate, dDate, rDate, stat);
    }
};

// Entity 5: FINE
class Fine {
private:
    int fineId;
    int loanId;
    string studentRoll;
    int overdueDays;
    double fineAmount; // ₹10 per day late
    string paymentStatus; // "PENDING", "PAID"

public:
    Fine() : fineId(0), loanId(0), overdueDays(0), fineAmount(0.0), paymentStatus("PENDING") {}
    Fine(int fId, int lId, string roll, int days, double amount, string pStatus = "PENDING")
        : fineId(fId), loanId(lId), studentRoll(roll), overdueDays(days), fineAmount(amount), paymentStatus(pStatus) {}

    int getFineId() const { return fineId; }
    int getLoanId() const { return loanId; }
    string getStudentRoll() const { return studentRoll; }
    int getOverdueDays() const { return overdueDays; }
    double getFineAmount() const { return fineAmount; }
    string getPaymentStatus() const { return paymentStatus; }
    void markPaid() { paymentStatus = "PAID"; }

    string toRow() const {
        return to_string(fineId) + "|" + to_string(loanId) + "|" + studentRoll + "|" +
               to_string(overdueDays) + "|" + to_string(static_cast<int>(fineAmount)) + "|" + paymentStatus;
    }

    static Fine fromRow(const string& row) {
        stringstream ss(row);
        string fIdStr, lIdStr, roll, daysStr, amtStr, stat;
        getline(ss, fIdStr, '|');
        getline(ss, lIdStr, '|');
        getline(ss, roll, '|');
        getline(ss, daysStr, '|');
        getline(ss, amtStr, '|');
        getline(ss, stat, '|');

        int fId = fIdStr.empty() ? 0 : stoi(fIdStr);
        int lId = lIdStr.empty() ? 0 : stoi(lIdStr);
        int days = daysStr.empty() ? 0 : stoi(daysStr);
        double amt = amtStr.empty() ? 0.0 : stod(amtStr);
        if (stat.empty()) stat = "PENDING";

        return Fine(fId, lId, roll, days, amt, stat);
    }
};

// ==========================================
// 2. DATE UTILITIES
// ==========================================

string getTodayDate() {
    time_t t = time(nullptr);
    tm* now = localtime(&t);
    char buf[20];
    strftime(buf, sizeof(buf), "%Y-%m-%d", now);
    return string(buf);
}

int calculateDaysBetween(const string& fromDate, const string& toDate) {
    tm tm1 = {}, tm2 = {};
    sscanf(fromDate.c_str(), "%d-%d-%d", &tm1.tm_year, &tm1.tm_mon, &tm1.tm_mday);
    sscanf(toDate.c_str(), "%d-%d-%d", &tm2.tm_year, &tm2.tm_mon, &tm2.tm_mday);

    tm1.tm_year -= 1900;
    tm1.tm_mon -= 1;
    tm2.tm_year -= 1900;
    tm2.tm_mon -= 1;

    time_t t1 = mktime(&tm1);
    time_t t2 = mktime(&tm2);
    if (t1 == -1 || t2 == -1) return 0;

    double diff = difftime(t2, t1);
    return static_cast<int>(diff / (60 * 60 * 24));
}

// Clear cin safely
void clearInput() {
    cin.clear();
    string dummy;
    getline(cin, dummy);
}

// ==========================================
// 3. DATABASE MANAGER (5-TABLE RELATIONAL FILE DB)
// ==========================================

class LabDatabase {
private:
    string labsFile = "labs.txt";
    string studentsFile = "students.txt";
    string equipmentFile = "equipment.txt";
    string loansFile = "loans.txt";
    string finesFile = "fines.txt";

public:
    LabDatabase() {
        initDatabase();
    }

    void initDatabase() {
        // 1. Seed Labs if empty
        ifstream inLab(labsFile);
        if (!inLab.is_open() || inLab.peek() == ifstream::traits_type::eof()) {
            inLab.close();
            ofstream out(labsFile);
            out << "LAB-IOT|IoT & Robotics Lab|Block B - Room 304|Prof. S. K. Verma\n";
            out << "LAB-ECE|Advanced Electronics Lab|Block A - Room 102|Dr. Anita Rao\n";
            out << "LAB-VLSI|Embedded & VLSI Design Lab|Block B - Room 308|Dr. R. K. Sharma\n";
            out.close();
        } else inLab.close();

        // 2. Seed Students if empty
        ifstream inStd(studentsFile);
        if (!inStd.is_open() || inStd.peek() == ifstream::traits_type::eof()) {
            inStd.close();
            ofstream out(studentsFile);
            out << "23CS101|Aarav Sharma|CSE|aarav@campus.edu|2\n";
            out << "23CS102|Bhavna Patel|CSE|bhavna@campus.edu|2\n";
            out << "23EC045|Chirag Verma|ECE|chirag@campus.edu|2\n";
            out << "23EC078|Divya Nair|ECE|divya@campus.edu|2\n";
            out.close();
        } else inStd.close();

        // 3. Seed Equipment if empty
        ifstream inEq(equipmentFile);
        if (!inEq.is_open() || inEq.peek() == ifstream::traits_type::eof()) {
            inEq.close();
            ofstream out(equipmentFile);
            out << "101|Arduino Uno R3 Kit|Microcontrollers|LAB-IOT|5|0|GOOD\n";
            out << "102|Digital Storage Oscilloscope|Test & Measurement|LAB-ECE|2|0|GOOD\n";
            out << "103|Raspberry Pi 4 Model B (4GB)|Single Board Computers|LAB-IOT|4|0|GOOD\n";
            out << "104|Digital Multimeter (True-RMS)|Instruments|LAB-ECE|6|0|GOOD\n";
            out << "105|FPGA Spartan-7 Board|VLSI Boards|LAB-VLSI|2|0|GOOD\n";
            out.close();
        } else inEq.close();

        // 4. Ensure Loans & Fines files exist
        ofstream outL(loansFile, ios::app); outL.close();
        ofstream outF(finesFile, ios::app); outF.close();
    }

    // Loaders
    vector<Lab> loadLabs() {
        vector<Lab> list;
        ifstream in(labsFile);
        string line;
        while (getline(in, line)) if (!line.empty()) list.push_back(Lab::fromRow(line));
        return list;
    }

    vector<Student> loadStudents() {
        vector<Student> list;
        ifstream in(studentsFile);
        string line;
        while (getline(in, line)) if (!line.empty()) list.push_back(Student::fromRow(line));
        return list;
    }

    vector<Equipment> loadEquipment() {
        vector<Equipment> list;
        ifstream in(equipmentFile);
        string line;
        while (getline(in, line)) if (!line.empty()) list.push_back(Equipment::fromRow(line));
        return list;
    }

    vector<Loan> loadLoans() {
        vector<Loan> list;
        ifstream in(loansFile);
        string line;
        while (getline(in, line)) if (!line.empty()) list.push_back(Loan::fromRow(line));
        return list;
    }

    vector<Fine> loadFines() {
        vector<Fine> list;
        ifstream in(finesFile);
        string line;
        while (getline(in, line)) if (!line.empty()) list.push_back(Fine::fromRow(line));
        return list;
    }

    // Savers
    void saveStudents(const vector<Student>& list) {
        ofstream out(studentsFile, ios::trunc);
        for (const auto& s : list) out << s.toRow() << "\n";
    }

    void saveEquipment(const vector<Equipment>& list) {
        ofstream out(equipmentFile, ios::trunc);
        for (const auto& e : list) out << e.toRow() << "\n";
    }

    void saveLoans(const vector<Loan>& list) {
        ofstream out(loansFile, ios::trunc);
        for (const auto& l : list) out << l.toRow() << "\n";
    }

    void saveFines(const vector<Fine>& list) {
        ofstream out(finesFile, ios::trunc);
        for (const auto& f : list) out << f.toRow() << "\n";
    }

    // Lookups
    Student* findStudent(vector<Student>& list, const string& roll) {
        for (auto& s : list) if (s.getRollNo() == roll) return &s;
        return nullptr;
    }

    Equipment* findEquipment(vector<Equipment>& list, int id) {
        for (auto& e : list) if (e.getId() == id) return &e;
        return nullptr;
    }

    Lab* findLab(vector<Lab>& list, const string& id) {
        for (auto& l : list) if (l.getLabId() == id) return &l;
        return nullptr;
    }

    int countActiveLoansForStudent(const string& roll) {
        auto loans = loadLoans();
        int count = 0;
        for (const auto& l : loans) {
            if (l.getStudentRoll() == roll && !l.isReturned()) count++;
        }
        return count;
    }

    // Business Operation: Add Student
    bool addStudent(string roll, string name, string dept, string email) {
        auto list = loadStudents();
        if (findStudent(list, roll) != nullptr) return false;
        list.push_back(Student(roll, name, dept, email, 2));
        saveStudents(list);
        return true;
    }

    // Business Operation: Add Equipment
    int addEquipment(string name, string cat, string labId, int qty) {
        auto list = loadEquipment();
        int newId = list.empty() ? 101 : list.back().getId() + 1;
        list.push_back(Equipment(newId, name, cat, labId, qty, 0, "GOOD"));
        saveEquipment(list);
        return newId;
    }

    // Business Operation: Issue Loan with Quota Validation
    // Return codes:
    //  > 0 : Success (Loan ID)
    //  -1  : Student not registered
    //  -2  : Student exceeded borrow limit
    //  -3  : Equipment not found
    //  -4  : Out of stock
    //  -5  : Equipment under maintenance
    int issueLoan(string roll, int eqId, string dueDate, string todayDate) {
        auto students = loadStudents();
        Student* s = findStudent(students, roll);
        if (!s) return -1; // Unregistered

        int activeLoans = countActiveLoansForStudent(roll);
        if (activeLoans >= s->getMaxBorrowLimit()) return -2; // Quota exceeded

        auto equipment = loadEquipment();
        Equipment* eq = findEquipment(equipment, eqId);
        if (!eq) return -3; // Equipment not found
        if (eq->getCondition() != "GOOD") return -5; // Maintenance
        if (eq->getAvailableQuantity() <= 0) return -4; // Stock 0

        // Increment equipment borrowed count
        eq->incrementBorrowed();
        saveEquipment(equipment);

        // Generate Loan ID
        auto loans = loadLoans();
        int newLoanId = loans.empty() ? 1001 : loans.back().getLoanId() + 1;

        Loan newLoan(newLoanId, eqId, roll, todayDate, dueDate, "PENDING", "ACTIVE");
        loans.push_back(newLoan);
        saveLoans(loans);

        return newLoanId;
    }

    // Business Operation: Return Loan with Automated Late Fine Calculation
    // Return codes:
    //   1: Success (No Fine)
    //   2: Success (Late Fine Incurred)
    //   0: Already Returned
    //  -1: Loan ID Not Found
    int returnLoan(int loanId, string todayDate, string& outEqName, int& outOverdueDays, double& outFineAmt) {
        auto loans = loadLoans();
        Loan* target = nullptr;
        for (auto& l : loans) {
            if (l.getLoanId() == loanId) {
                target = &l;
                break;
            }
        }

        if (!target) return -1;
        if (target->isReturned()) return 0;

        // Get Equipment details
        auto equipment = loadEquipment();
        Equipment* eq = findEquipment(equipment, target->getEquipmentId());
        outEqName = eq ? eq->getName() : "Unknown Hardware";

        // Mark returned
        target->markReturned(todayDate);
        saveLoans(loans);

        // Restore equipment stock
        if (eq) {
            eq->decrementBorrowed();
            saveEquipment(equipment);
        }

        // Calculate Overdue Days and Late Fine (₹10/day)
        int days = calculateDaysBetween(target->getDueDate(), todayDate);
        outOverdueDays = days > 0 ? days : 0;
        outFineAmt = outOverdueDays * 10.0; // ₹10 per day late fine

        if (outOverdueDays > 0) {
            auto fines = loadFines();
            int newFineId = fines.empty() ? 501 : fines.back().getFineId() + 1;
            fines.push_back(Fine(newFineId, loanId, target->getStudentRoll(), outOverdueDays, outFineAmt, "PENDING"));
            saveFines(fines);
            return 2; // Incurred fine
        }

        return 1; // Returned on time
    }

    // Pay Fine
    bool payFine(int fineId) {
        auto fines = loadFines();
        for (auto& f : fines) {
            if (f.getFineId() == fineId) {
                f.markPaid();
                saveFines(fines);
                return true;
            }
        }
        return false;
    }

    string getLabsPath() const { return labsFile; }
    string getStudentsPath() const { return studentsFile; }
    string getEquipmentPath() const { return equipmentFile; }
    string getLoansPath() const { return loansFile; }
    string getFinesPath() const { return finesFile; }
};

// ==========================================
// 4. PRESENTATION / MENU SYSTEM
// ==========================================

void printHeader(const string& title) {
    cout << "\n========================================================================\n";
    cout << "  " << title << "\n";
    cout << "========================================================================\n";
}

void printDivider() {
    cout << "------------------------------------------------------------------------\n";
}

void viewLabsAndLocations(LabDatabase& db) {
    printHeader("CAMPUS LABS & PHYSICAL ROOM LOCATIONS");
    auto labs = db.loadLabs();
    auto equipment = db.loadEquipment();

    cout << left << setw(12) << "Lab ID"
                 << setw(30) << "Lab Name"
                 << setw(22) << "Location/Room"
                 << setw(20) << "Faculty In-Charge" << "\n";
    printDivider();

    for (const auto& l : labs) {
        cout << left << setw(12) << l.getLabId()
                     << setw(30) << l.getLabName()
                     << setw(22) << l.getRoomNo()
                     << setw(20) << l.getInchargeFaculty() << "\n";
    }
    printDivider();
}

void viewEquipmentInventory(LabDatabase& db) {
    printHeader("LAB HARDWARE INVENTORY (WITH ROOM LOCATIONS)");
    auto eqList = db.loadEquipment();
    auto labs = db.loadLabs();

    cout << left << setw(6)  << "ID"
                 << setw(28) << "Equipment Name"
                 << setw(16) << "Category"
                 << setw(10) << "Lab ID"
                 << setw(7)  << "Total"
                 << setw(7)  << "Avail"
                 << setw(12) << "Condition" << "\n";
    printDivider();

    for (const auto& eq : eqList) {
        cout << left << setw(6)  << eq.getId()
                     << setw(28) << (eq.getName().length() > 26 ? eq.getName().substr(0, 24) + ".." : eq.getName())
                     << setw(16) << (eq.getCategory().length() > 14 ? eq.getCategory().substr(0, 12) + ".." : eq.getCategory())
                     << setw(10) << eq.getLabId()
                     << setw(7)  << eq.getTotalQuantity()
                     << setw(7)  << eq.getAvailableQuantity()
                     << setw(12) << eq.getCondition() << "\n";
    }
    printDivider();
}

void viewRegisteredStudents(LabDatabase& db) {
    printHeader("REGISTERED STUDENTS & BORROW QUOTAS");
    auto students = db.loadStudents();

    cout << left << setw(12) << "Roll No"
                 << setw(20) << "Student Name"
                 << setw(8)  << "Dept"
                 << setw(22) << "Contact Email"
                 << setw(10) << "Active"
                 << setw(8)  << "Limit" << "\n";
    printDivider();

    for (const auto& s : students) {
        int active = db.countActiveLoansForStudent(s.getRollNo());
        cout << left << setw(12) << s.getRollNo()
                     << setw(20) << s.getName()
                     << setw(8)  << s.getDepartment()
                     << setw(22) << s.getContact()
                     << setw(10) << (to_string(active) + " units")
                     << setw(8)  << s.getMaxBorrowLimit() << "\n";
    }
    printDivider();
}

void registerNewStudent(LabDatabase& db) {
    printHeader("REGISTER NEW STUDENT");
    cout << "  Enter University Roll Number : ";
    string roll; getline(cin, roll);
    if (roll.empty()) { cout << "  [!] Roll number cannot be empty.\n"; return; }

    cout << "  Enter Student Full Name      : ";
    string name; getline(cin, name);
    if (name.empty()) { cout << "  [!] Name cannot be empty.\n"; return; }

    cout << "  Enter Department (CSE/ECE)   : ";
    string dept; getline(cin, dept);

    cout << "  Enter Contact Email          : ";
    string email; getline(cin, email);

    if (db.addStudent(roll, name, dept, email)) {
        cout << "\n  [✓] SUCCESS: Student '" << name << "' (" << roll << ") registered!\n";
        cout << "      Default borrow quota: 2 hardware units.\n";
    } else {
        cout << "\n  [!] Error: Student with Roll Number '" << roll << "' already exists!\n";
    }
}

void addNewEquipment(LabDatabase& db) {
    printHeader("ADD NEW LAB HARDWARE EQUIPMENT");
    cout << "  Enter Equipment Name : ";
    string name; getline(cin, name);
    if (name.empty()) { cout << "  [!] Name cannot be empty.\n"; return; }

    cout << "  Enter Category       : ";
    string cat; getline(cin, cat);
    if (cat.empty()) cat = "Sensors & Boards";

    viewLabsAndLocations(db);
    cout << "  Enter Lab ID to Assign Item (e.g., LAB-IOT, LAB-ECE) : ";
    string labId; getline(cin, labId);
    if (labId.empty()) labId = "LAB-IOT";

    cout << "  Enter Quantity       : ";
    int qty;
    if (!(cin >> qty) || qty <= 0) {
        clearInput();
        cout << "  [!] Invalid quantity entered.\n";
        return;
    }
    clearInput();

    int id = db.addEquipment(name, cat, labId, qty);
    cout << "\n  [✓] SUCCESS: Added '" << name << "' (ID: #" << id 
         << ") with " << qty << " units assigned to " << labId << ".\n";
}

void issueEquipment(LabDatabase& db) {
    printHeader("ISSUE HARDWARE TO STUDENT (QUOTA PROTECTED)");
    viewEquipmentInventory(db);

    cout << "\n  Enter Equipment ID to Borrow : ";
    int eqId;
    if (!(cin >> eqId)) { clearInput(); cout << "  [!] Invalid ID.\n"; return; }
    clearInput();

    cout << "  Enter Student Roll Number    : ";
    string roll; getline(cin, roll);
    if (roll.empty()) { cout << "  [!] Roll number cannot be empty.\n"; return; }

    string today = getTodayDate();
    cout << "  Today's Issue Date           : " << today << "\n";

    cout << "  Enter Due Date (YYYY-MM-DD)  [Press Enter for +7 days]: ";
    string dueDate; getline(cin, dueDate);
    if (dueDate.empty()) dueDate = today;

    int res = db.issueLoan(roll, eqId, dueDate, today);

    if (res > 0) {
        cout << "\n  [✓] LOAN CONFIRMED!\n";
        cout << "      Loan Receipt ID : #" << res << "\n";
        cout << "      Student Roll    : " << roll << "\n";
        cout << "      Due Date        : " << dueDate << "\n";
        cout << "      Status          : ACTIVE (Transaction saved to disk)\n";
    } else if (res == -1) {
        cout << "\n  [x] REJECTED: Student '" << roll << "' is not registered in the system!\n";
        cout << "      Please register the student first (Menu Option 3).\n";
    } else if (res == -2) {
        cout << "\n  [x] REJECTED (QUOTA EXCEEDED): Student '" << roll 
             << "' has already borrowed their limit (Max 2 units)!\n";
        cout << "      Student must return an existing item before borrowing new hardware.\n";
    } else if (res == -3) {
        cout << "\n  [x] Error: Equipment ID #" << eqId << " does not exist.\n";
    } else if (res == -4) {
        cout << "\n  [!] OUT OF STOCK: All units of this hardware are currently on loan!\n";
    } else if (res == -5) {
        cout << "\n  [!] HARDWARE BLOCKED: This equipment is currently marked under MAINTENANCE.\n";
    }
}

void returnEquipment(LabDatabase& db) {
    printHeader("RETURN HARDWARE (WITH AUTOMATED FINE CALCULATOR)");
    cout << "  Enter Loan Receipt ID to Return: ";
    int loanId;
    if (!(cin >> loanId)) { clearInput(); cout << "  [!] Invalid Loan ID.\n"; return; }
    clearInput();

    string today = getTodayDate();
    string eqName;
    int overdueDays = 0;
    double fineAmt = 0.0;

    int res = db.returnLoan(loanId, today, eqName, overdueDays, fineAmt);

    if (res == 1) {
        cout << "\n  [✓] RETURN SUCCESSFUL!\n";
        cout << "      Equipment   : " << eqName << "\n";
        cout << "      Return Date : " << today << " (On-Time)\n";
        cout << "      Late Fine   : ₹0.00 (No penalty)\n";
        cout << "      Stock restored to inventory automatically.\n";
    } else if (res == 2) {
        cout << "\n  [!] RETURN ACCEPTED WITH OVERDUE PENALTY!\n";
        cout << "      Equipment    : " << eqName << "\n";
        cout << "      Return Date  : " << today << "\n";
        cout << "      Days Overdue : " << overdueDays << " day(s) late\n";
        cout << "      Late Fine    : ₹" << static_cast<int>(fineAmt) << " (Rate: ₹10/day)\n";
        cout << "      Status       : PENDING PAYMENT (Recorded in fines table)\n";
    } else if (res == 0) {
        cout << "\n  [!] DUPLICATE RETURN ATTEMPT: Loan #" << loanId << " was already returned earlier!\n";
    } else {
        cout << "\n  [x] Error: Loan ID #" << loanId << " not found in database.\n";
    }
}

void viewActiveAndOverdueLoans(LabDatabase& db) {
    printHeader("ACTIVE & OVERDUE HARDWARE LOANS");
    auto loans = db.loadLoans();
    auto equipment = db.loadEquipment();
    string today = getTodayDate();

    vector<Loan> activeLoans;
    for (const auto& l : loans) {
        if (!l.isReturned()) activeLoans.push_back(l);
    }

    if (activeLoans.empty()) {
        cout << "  No active loans. All lab hardware is currently in stock.\n";
        return;
    }

    cout << left << setw(8)  << "Loan ID"
                 << setw(12) << "Roll No"
                 << setw(8)  << "EquipID"
                 << setw(12) << "Issue Date"
                 << setw(12) << "Due Date"
                 << setw(14) << "Status" << "\n";
    printDivider();

    int overdueCount = 0;
    for (const auto& l : activeLoans) {
        string status = "ACTIVE";
        int days = calculateDaysBetween(l.getDueDate(), today);
        if (days > 0) {
            status = "OVERDUE (" + to_string(days) + "d)";
            overdueCount++;
        }

        cout << left << setw(8)  << ("#" + to_string(l.getLoanId()))
                     << setw(12) << l.getStudentRoll()
                     << setw(8)  << l.getEquipmentId()
                     << setw(12) << l.getIssueDate()
                     << setw(12) << l.getDueDate()
                     << setw(14) << status << "\n";
    }
    printDivider();
    cout << "  Total Active: " << activeLoans.size() << " | Overdue: " << overdueCount << "\n";
}

void viewAndPayFines(LabDatabase& db) {
    printHeader("STUDENT LATE FINES & PENALTIES");
    auto fines = db.loadFines();

    if (fines.empty()) {
        cout << "  No fines on record. All students have returned equipment on time!\n";
        return;
    }

    cout << left << setw(8)  << "Fine ID"
                 << setw(10) << "Loan ID"
                 << setw(12) << "Roll No"
                 << setw(12) << "Overdue Days"
                 << setw(12) << "Amount (₹)"
                 << setw(12) << "Status" << "\n";
    printDivider();

    for (const auto& f : fines) {
        cout << left << setw(8)  << ("#" + to_string(f.getFineId()))
                     << setw(10) << ("#" + to_string(f.getLoanId()))
                     << setw(12) << f.getStudentRoll()
                     << setw(12) << (to_string(f.getOverdueDays()) + " days")
                     << setw(12) << ("₹" + to_string(static_cast<int>(f.getFineAmount())))
                     << setw(12) << f.getPaymentStatus() << "\n";
    }
    printDivider();

    cout << "  Do you want to mark a fine as PAID? (y/n): ";
    char ch;
    if (cin >> ch && (ch == 'y' || ch == 'Y')) {
        cout << "  Enter Fine ID to pay: ";
        int fId;
        if (cin >> fId) {
            clearInput();
            if (db.payFine(fId)) {
                cout << "  [✓] Fine #" << fId << " marked as PAID!\n";
            } else {
                cout << "  [x] Fine ID not found.\n";
            }
        }
    } else {
        clearInput();
    }
}

void inspectAllDatabaseFiles(LabDatabase& db) {
    printHeader("FACULTY AUDIT: RAW 3NF DATABASE FILES ON DISK");

    auto printFile = [](const string& name, const string& path) {
        cout << "\n--> Table: " << name << " (" << path << ")\n";
        ifstream in(path);
        string line;
        bool hasData = false;
        while (getline(in, line)) {
            cout << "    " << line << "\n";
            hasData = true;
        }
        if (!hasData) cout << "    (Empty)\n";
        in.close();
    };

    printFile("1. LABS", db.getLabsPath());
    printFile("2. STUDENTS", db.getStudentsPath());
    printFile("3. EQUIPMENT", db.getEquipmentPath());
    printFile("4. LOANS", db.getLoansPath());
    printFile("5. FINES", db.getFinesPath());

    printDivider();
    cout << "  All 5 tables are stored as relational text files with primary/foreign keys.\n";
    cout << "  You can also inspect or edit them using Notepad or Excel!\n";
}

// ==========================================
// 5. MAIN MENU LOOP
// ==========================================

int main() {
    LabDatabase db;

    cout << "\n========================================================================\n";
    cout << "   CAMPUS LAB EQUIPMENT BORROWING SYSTEM (C++ OOP & RELATIONAL DB)     \n";
    cout << "========================================================================\n";
    cout << "  [✓] Relational Database initialized (5 Tables: Labs, Students, Equip, Loans, Fines)\n";

    while (true) {
        cout << "\n========================== MAIN MENU ==========================\n";
        cout << "  1. View Equipment Inventory & Live Stock\n";
        cout << "  2. View Lab Rooms & Physical Locations\n";
        cout << "  3. View / Register Students (Quota: 2 Units)\n";
        cout << "  4. Issue Hardware to Student (Borrow)\n";
        cout << "  5. Return Hardware (Automated Fine Calculator)\n";
        cout << "  6. View Active & Overdue Hardware Loans\n";
        cout << "  7. View Late Fines & Mark Payments\n";
        cout << "  8. Add New Equipment to a Lab\n";
        cout << "  9. Inspect All 5 Raw Database Files (Show Faculty)\n";
        cout << "  10. Exit System\n";
        cout << "===============================================================\n";
        cout << "  Select option [1-10]: ";

        int choice;
        if (!(cin >> choice)) {
            cout << "\n  Closing system safely. Goodbye!\n";
            break;
        }
        clearInput();

        switch (choice) {
            case 1: viewEquipmentInventory(db); break;
            case 2: viewLabsAndLocations(db); break;
            case 3: {
                viewRegisteredStudents(db);
                cout << "  Do you want to register a new student? (y/n): ";
                char c;
                if (cin >> c && (c == 'y' || c == 'Y')) {
                    clearInput();
                    registerNewStudent(db);
                } else clearInput();
                break;
            }
            case 4: issueEquipment(db); break;
            case 5: returnEquipment(db); break;
            case 6: viewActiveAndOverdueLoans(db); break;
            case 7: viewAndPayFines(db); break;
            case 8: addNewEquipment(db); break;
            case 9: inspectAllDatabaseFiles(db); break;
            case 10:
                cout << "\n  All transactions saved permanently to disk. Goodbye!\n\n";
                return 0;
            default:
                cout << "  [!] Please select a valid number from 1 to 10.\n";
                break;
        }
    }

    return 0;
}
