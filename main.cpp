#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>
#include <ctime>
#include <cstdlib>

using namespace std;

// ==========================================
// 1. DOMAIN CLASSES (OOP ENCAPSULATION)
// ==========================================

class Equipment {
private:
    int id;
    string name;
    string category;
    int totalQuantity;
    int borrowedCount;

public:
    Equipment() : id(0), name(""), category(""), totalQuantity(0), borrowedCount(0) {}
    
    Equipment(int id, string name, string category, int total, int borrowed = 0)
        : id(id), name(name), category(category), totalQuantity(total), borrowedCount(borrowed) {}

    // Getters
    int getId() const { return id; }
    string getName() const { return name; }
    string getCategory() const { return category; }
    int getTotalQuantity() const { return totalQuantity; }
    int getBorrowedCount() const { return borrowedCount; }
    int getAvailableQuantity() const { 
        int avail = totalQuantity - borrowedCount;
        return avail > 0 ? avail : 0;
    }

    // Setters
    void setBorrowedCount(int b) { borrowedCount = b; }
    void incrementBorrowed() { borrowedCount++; }
    void decrementBorrowed() { if (borrowedCount > 0) borrowedCount--; }

    // Serialization for Database File
    string toDatabaseRow() const {
        return to_string(id) + "|" + name + "|" + category + "|" + 
               to_string(totalQuantity) + "|" + to_string(borrowedCount);
    }

    static Equipment fromDatabaseRow(const string& row) {
        stringstream ss(row);
        string idStr, name, cat, totalStr, borrowStr;
        
        getline(ss, idStr, '|');
        getline(ss, name, '|');
        getline(ss, cat, '|');
        getline(ss, totalStr, '|');
        getline(ss, borrowStr, '|');

        int id = idStr.empty() ? 0 : stoi(idStr);
        int total = totalStr.empty() ? 0 : stoi(totalStr);
        int borrowed = borrowStr.empty() ? 0 : stoi(borrowStr);

        return Equipment(id, name, cat, total, borrowed);
    }
};

class LoanRecord {
private:
    int loanId;
    int equipmentId;
    string equipmentName;
    string studentRoll;
    string studentName;
    string department;
    string borrowDate;
    string dueDate;
    string returnDate; // "PENDING" if not returned

public:
    LoanRecord() : loanId(0), equipmentId(0), returnDate("PENDING") {}

    LoanRecord(int lId, int eId, string eName, string roll, string sName, 
               string dept, string bDate, string dDate, string rDate = "PENDING")
        : loanId(lId), equipmentId(eId), equipmentName(eName), studentRoll(roll),
          studentName(sName), department(dept), borrowDate(bDate), dueDate(dDate), returnDate(rDate) {}

    // Getters
    int getLoanId() const { return loanId; }
    int getEquipmentId() const { return equipmentId; }
    string getEquipmentName() const { return equipmentName; }
    string getStudentRoll() const { return studentRoll; }
    string getStudentName() const { return studentName; }
    string getDepartment() const { return department; }
    string getBorrowDate() const { return borrowDate; }
    string getDueDate() const { return dueDate; }
    string getReturnDate() const { return returnDate; }
    bool isReturned() const { return returnDate != "PENDING"; }

    void markReturned(const string& date) { returnDate = date; }

    // Serialization for Database File
    string toDatabaseRow() const {
        return to_string(loanId) + "|" + to_string(equipmentId) + "|" + equipmentName + "|" +
               studentRoll + "|" + studentName + "|" + department + "|" +
               borrowDate + "|" + dueDate + "|" + returnDate;
    }

    static LoanRecord fromDatabaseRow(const string& row) {
        stringstream ss(row);
        string lIdStr, eIdStr, eName, roll, sName, dept, bDate, dDate, rDate;

        getline(ss, lIdStr, '|');
        getline(ss, eIdStr, '|');
        getline(ss, eName, '|');
        getline(ss, roll, '|');
        getline(ss, sName, '|');
        getline(ss, dept, '|');
        getline(ss, bDate, '|');
        getline(ss, dDate, '|');
        getline(ss, rDate, '|');

        int lId = lIdStr.empty() ? 0 : stoi(lIdStr);
        int eId = eIdStr.empty() ? 0 : stoi(eIdStr);
        if (rDate.empty()) rDate = "PENDING";

        return LoanRecord(lId, eId, eName, roll, sName, dept, bDate, dDate, rDate);
    }
};

// ==========================================
// 2. DATABASE MANAGER (FILE-BASED DATABASE)
// ==========================================

class LabDatabase {
private:
    string equipmentFile;
    string loansFile;

public:
    LabDatabase(string eqFile = "equipment_db.txt", string lnFile = "loans_db.txt")
        : equipmentFile(eqFile), loansFile(lnFile) {
        initDatabase();
    }

    void initDatabase() {
        // If equipment table file doesn't exist, create and seed with default college lab items
        ifstream checkEq(equipmentFile);
        if (!checkEq.is_open() || checkEq.peek() == ifstream::traits_type::eof()) {
            checkEq.close();
            ofstream outEq(equipmentFile);
            if (outEq.is_open()) {
                vector<Equipment> seedData = {
                    Equipment(101, "Arduino Uno R3 Kit", "Embedded Systems", 5, 0),
                    Equipment(102, "Digital Storage Oscilloscope", "Electronics", 2, 0),
                    Equipment(103, "Raspberry Pi 4 Model B", "IoT & Robotics", 4, 0),
                    Equipment(104, "Digital Multimeter", "Test & Measurement", 6, 0),
                    Equipment(105, "FPGA Spartan-7 Board", "VLSI Design", 2, 0)
                };
                for (const auto& item : seedData) {
                    outEq << item.toDatabaseRow() << "\n";
                }
                outEq.close();
            }
        } else {
            checkEq.close();
        }

        // Check if loans table file exists
        ifstream checkLn(loansFile);
        if (!checkLn.is_open()) {
            ofstream outLn(loansFile);
            outLn.close();
        } else {
            checkLn.close();
        }
    }

    // Read all equipment from database
    vector<Equipment> loadAllEquipment() {
        vector<Equipment> list;
        ifstream in(equipmentFile);
        string line;
        while (getline(in, line)) {
            if (!line.empty()) {
                list.push_back(Equipment::fromDatabaseRow(line));
            }
        }
        in.close();
        return list;
    }

    // Save all equipment back to database
    void saveAllEquipment(const vector<Equipment>& list) {
        ofstream out(equipmentFile, ios::trunc);
        for (const auto& eq : list) {
            out << eq.toDatabaseRow() << "\n";
        }
        out.close();
    }

    // Add new equipment
    int addEquipment(string name, string category, int quantity) {
        auto list = loadAllEquipment();
        int newId = 101;
        if (!list.empty()) {
            newId = list.back().getId() + 1;
        }
        Equipment newEq(newId, name, category, quantity, 0);
        list.push_back(newEq);
        saveAllEquipment(list);
        return newId;
    }

    // Read all loans from database
    vector<LoanRecord> loadAllLoans() {
        vector<LoanRecord> list;
        ifstream in(loansFile);
        string line;
        while (getline(in, line)) {
            if (!line.empty()) {
                list.push_back(LoanRecord::fromDatabaseRow(line));
            }
        }
        in.close();
        return list;
    }

    // Save all loans back to database
    void saveAllLoans(const vector<LoanRecord>& list) {
        ofstream out(loansFile, ios::trunc);
        for (const auto& loan : list) {
            out << loan.toDatabaseRow() << "\n";
        }
        out.close();
    }

    // Issue equipment loan
    int issueLoan(int equipmentId, string roll, string name, string dept, string dueDate, string todayDate) {
        auto eqList = loadAllEquipment();
        int eqIndex = -1;

        for (size_t i = 0; i < eqList.size(); ++i) {
            if (eqList[i].getId() == equipmentId) {
                eqIndex = static_cast<int>(i);
                break;
            }
        }

        if (eqIndex == -1) {
            return -1; // Equipment not found
        }

        if (eqList[eqIndex].getAvailableQuantity() <= 0) {
            return -2; // Out of stock
        }

        // Update equipment inventory
        eqList[eqIndex].incrementBorrowed();
        saveAllEquipment(eqList);

        // Add loan record
        auto loanList = loadAllLoans();
        int newLoanId = 1;
        if (!loanList.empty()) {
            newLoanId = loanList.back().getLoanId() + 1;
        }

        LoanRecord record(newLoanId, equipmentId, eqList[eqIndex].getName(),
                          roll, name, dept, todayDate, dueDate, "PENDING");
        loanList.push_back(record);
        saveAllLoans(loanList);

        return newLoanId;
    }

    // Return equipment loan
    // Returns: 1 = Success, 0 = Already returned, -1 = Loan ID not found
    int returnLoan(int loanId, string todayDate, string& outEqName, string& outPrevDate) {
        auto loanList = loadAllLoans();
        int loanIndex = -1;

        for (size_t i = 0; i < loanList.size(); ++i) {
            if (loanList[i].getLoanId() == loanId) {
                loanIndex = static_cast<int>(i);
                break;
            }
        }

        if (loanIndex == -1) {
            return -1; // Not found
        }

        if (loanList[loanIndex].isReturned()) {
            outEqName = loanList[loanIndex].getEquipmentName();
            outPrevDate = loanList[loanIndex].getReturnDate();
            return 0; // Already returned
        }

        // Mark returned in loans
        loanList[loanIndex].markReturned(todayDate);
        outEqName = loanList[loanIndex].getEquipmentName();
        saveAllLoans(loanList);

        // Restore equipment stock
        int eqId = loanList[loanIndex].getEquipmentId();
        auto eqList = loadAllEquipment();
        for (auto& eq : eqList) {
            if (eq.getId() == eqId) {
                eq.decrementBorrowed();
                break;
            }
        }
        saveAllEquipment(eqList);

        return 1; // Success
    }

    string getEquipmentFilePath() const { return equipmentFile; }
    string getLoansFilePath() const { return loansFile; }
};

// ==========================================
// 3. UTILITY FUNCTIONS & DATE HELPERS
// ==========================================

string getTodayDate() {
    time_t t = time(nullptr);
    tm* now = localtime(&t);
    char buf[20];
    strftime(buf, sizeof(buf), "%Y-%m-%d", now);
    return string(buf);
}

bool isOverdue(const string& dueDate, const string& todayDate) {
    return dueDate < todayDate;
}

// Clear cin buffer safely
void clearInput() {
    cin.clear();
    string dummy;
    getline(cin, dummy);
}

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

void showAllEquipment(LabDatabase& db) {
    printHeader("LAB INVENTORY & AVAILABILITY STATUS");
    auto list = db.loadAllEquipment();

    if (list.empty()) {
        cout << "  No equipment records found.\n";
        return;
    }

    cout << left << setw(8)  << "ID"
                 << setw(32) << "Equipment Name"
                 << setw(20) << "Category"
                 << setw(8)  << "Total"
                 << setw(10) << "Borrowed"
                 << setw(10) << "Available" << "\n";
    printDivider();

    for (const auto& eq : list) {
        cout << left << setw(8)  << eq.getId()
                     << setw(32) << (eq.getName().length() > 30 ? eq.getName().substr(0, 27) + "..." : eq.getName())
                     << setw(20) << (eq.getCategory().length() > 18 ? eq.getCategory().substr(0, 15) + "..." : eq.getCategory())
                     << setw(8)  << eq.getTotalQuantity()
                     << setw(10) << eq.getBorrowedCount()
                     << setw(10) << eq.getAvailableQuantity() << "\n";
    }
    printDivider();
}

void addNewEquipment(LabDatabase& db) {
    printHeader("ADD NEW LAB EQUIPMENT");
    cout << "  Enter Equipment Name : ";
    string name;
    getline(cin, name);
    if (name.empty()) {
        cout << "  [!] Equipment name cannot be empty.\n";
        return;
    }

    cout << "  Enter Category       : ";
    string category;
    getline(cin, category);
    if (category.empty()) category = "General";

    cout << "  Enter Quantity       : ";
    int quantity;
    if (!(cin >> quantity) || quantity <= 0) {
        clearInput();
        cout << "  [!] Invalid quantity entered.\n";
        return;
    }
    clearInput();

    int id = db.addEquipment(name, category, quantity);
    cout << "\n  [✓] Success: Added '" << name << "' (Assigned ID: #" << id 
         << ") with " << quantity << " units.\n";
    cout << "  [✓] Saved directly to database file: " << db.getEquipmentFilePath() << "\n";
}

void issueEquipment(LabDatabase& db) {
    printHeader("ISSUE EQUIPMENT TO STUDENT");
    showAllEquipment(db);

    cout << "\n  Enter Equipment ID to Borrow : ";
    int eqId;
    if (!(cin >> eqId)) {
        clearInput();
        cout << "  [!] Invalid ID entered.\n";
        return;
    }
    clearInput();

    cout << "  Enter Student Roll Number    : ";
    string roll;
    getline(cin, roll);
    if (roll.empty()) {
        cout << "  [!] Roll number cannot be empty.\n";
        return;
    }

    cout << "  Enter Student Name           : ";
    string name;
    getline(cin, name);
    if (name.empty()) name = "Student " + roll;

    cout << "  Enter Department / Class     : ";
    string dept;
    getline(cin, dept);
    if (dept.empty()) dept = "CSE / ECE";

    string today = getTodayDate();
    cout << "  Today's Date                 : " << today << "\n";

    cout << "  Enter Return Due Date (YYYY-MM-DD) [Press Enter for Today]: ";
    string dueDate;
    getline(cin, dueDate);
    if (dueDate.empty()) dueDate = today;

    int loanId = db.issueLoan(eqId, roll, name, dept, dueDate, today);

    if (loanId == -1) {
        cout << "\n  [x] Error: Equipment ID #" << eqId << " not found in database!\n";
    } else if (loanId == -2) {
        cout << "\n  [!] Cannot Issue: All units of this equipment are currently on loan (Stock = 0)!\n";
    } else {
        cout << "\n  [✓] LOAN CONFIRMED!\n";
        cout << "      Loan Receipt ID : #" << loanId << "\n";
        cout << "      Student         : " << name << " (" << roll << ")\n";
        cout << "      Department      : " << dept << "\n";
        cout << "      Due Date        : " << dueDate << "\n";
        cout << "      Status          : ACTIVE (Database updated)\n";
    }
}

void returnEquipment(LabDatabase& db) {
    printHeader("RETURN BORROWED EQUIPMENT");
    cout << "  Enter Loan ID to Return: ";
    int loanId;
    if (!(cin >> loanId)) {
        clearInput();
        cout << "  [!] Invalid Loan ID.\n";
        return;
    }
    clearInput();

    string today = getTodayDate();
    string eqName, prevDate;
    int res = db.returnLoan(loanId, today, eqName, prevDate);

    if (res == 1) {
        cout << "\n  [✓] RETURN SUCCESSFUL!\n";
        cout << "      Equipment: " << eqName << "\n";
        cout << "      Return Date: " << today << "\n";
        cout << "      Inventory stock has been incremented and database updated.\n";
    } else if (res == 0) {
        cout << "\n  [!] ALREADY RETURNED!\n";
        cout << "      Loan #" << loanId << " (" << eqName << ") was already returned on " << prevDate << ".\n";
    } else {
        cout << "\n  [x] Error: Loan ID #" << loanId << " not found in database.\n";
    }
}

void viewActiveAndOverdueLoans(LabDatabase& db) {
    printHeader("ACTIVE & OVERDUE STUDENT LOANS");
    auto loans = db.loadAllLoans();
    string today = getTodayDate();

    vector<LoanRecord> activeLoans;
    for (const auto& l : loans) {
        if (!l.isReturned()) {
            activeLoans.push_back(l);
        }
    }

    if (activeLoans.empty()) {
        cout << "  No active loans. All equipment is present in the lab.\n";
        return;
    }

    cout << left << setw(6)  << "Loan"
                 << setw(12) << "Roll No"
                 << setw(18) << "Student"
                 << setw(24) << "Equipment"
                 << setw(12) << "Due Date"
                 << setw(14) << "Status" << "\n";
    printDivider();

    int overdueCount = 0;
    for (const auto& l : activeLoans) {
        string status = "ACTIVE";
        if (isOverdue(l.getDueDate(), today)) {
            status = "OVERDUE [!]";
            overdueCount++;
        }

        cout << left << setw(6)  << ("#" + to_string(l.getLoanId()))
                     << setw(12) << l.getStudentRoll()
                     << setw(18) << (l.getStudentName().length() > 16 ? l.getStudentName().substr(0, 14) + ".." : l.getStudentName())
                     << setw(24) << (l.getEquipmentName().length() > 22 ? l.getEquipmentName().substr(0, 20) + ".." : l.getEquipmentName())
                     << setw(12) << l.getDueDate()
                     << setw(14) << status << "\n";
    }
    printDivider();
    cout << "  Total Active Loans: " << activeLoans.size() << " | Overdue: " << overdueCount << "\n";
}

void displayRawDatabaseFiles(LabDatabase& db) {
    printHeader("FACULTY INSPECTION: RAW DATABASE FILES ON DISK");

    cout << "\n[1] Equipment Table File (" << db.getEquipmentFilePath() << "):\n";
    printDivider();
    ifstream eqIn(db.getEquipmentFilePath());
    string line;
    while (getline(eqIn, line)) {
        cout << "  " << line << "\n";
    }
    eqIn.close();

    cout << "\n[2] Loans Table File (" << db.getLoansFilePath() << "):\n";
    printDivider();
    ifstream lnIn(db.getLoansFilePath());
    bool hasLoans = false;
    while (getline(lnIn, line)) {
        cout << "  " << line << "\n";
        hasLoans = true;
    }
    if (!hasLoans) cout << "  (Empty table - no loans yet)\n";
    lnIn.close();
    printDivider();
    cout << "  Format: ID | Name | Category | TotalQuantity | BorrowedCount\n";
    cout << "  You can also open these text files in Notepad or Excel!\n";
}

// ==========================================
// 5. MAIN ENTRY POINT
// ==========================================

int main() {
    LabDatabase db;

    cout << "\n========================================================================\n";
    cout << "      CAMPUS LAB EQUIPMENT BORROWING SYSTEM (C++ OOP & DB)             \n";
    cout << "========================================================================\n";
    cout << "  Database loaded successfully from: " << db.getEquipmentFilePath() << "\n";

    while (true) {
        cout << "\n========================== MAIN MENU ==========================\n";
        cout << "  1. Add New Equipment to Inventory\n";
        cout << "  2. View Equipment & Stock Availability\n";
        cout << "  3. Issue Equipment to Student (Borrow)\n";
        cout << "  4. Return Equipment\n";
        cout << "  5. View Active & Overdue Loans\n";
        cout << "  6. Inspect Raw Database Files (Show Faculty)\n";
        cout << "  7. Exit Program\n";
        cout << "===============================================================\n";
        cout << "  Enter choice [1-7]: ";

        int choice;
        if (!(cin >> choice)) {
            cout << "\n  Exiting program. Goodbye!\n";
            break;
        }
        clearInput();

        switch (choice) {
            case 1:
                addNewEquipment(db);
                break;
            case 2:
                showAllEquipment(db);
                break;
            case 3:
                issueEquipment(db);
                break;
            case 4:
                returnEquipment(db);
                break;
            case 5:
                viewActiveAndOverdueLoans(db);
                break;
            case 6:
                displayRawDatabaseFiles(db);
                break;
            case 7:
                cout << "\n  Saving all records to database. Thank you!\n\n";
                return 0;
            default:
                cout << "  [!] Invalid choice. Please select 1 to 7.\n";
                break;
        }
    }

    return 0;
}
