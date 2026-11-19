#include "lending.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <chrono>
#include <regex>

namespace lab {

LabManager::LabManager(Database& db) : db_(db) {
    initializeSchema();
    seedSampleDataIfEmpty();
}

void LabManager::initializeSchema() {
    const std::string sql =
        "CREATE TABLE IF NOT EXISTS equipment ("
        "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    name TEXT NOT NULL UNIQUE,"
        "    category TEXT NOT NULL,"
        "    total_quantity INTEGER NOT NULL CHECK(total_quantity >= 0)"
        ");"
        "CREATE TABLE IF NOT EXISTS loans ("
        "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    equipment_id INTEGER NOT NULL,"
        "    student_roll TEXT NOT NULL,"
        "    student_name TEXT NOT NULL,"
        "    department TEXT NOT NULL,"
        "    borrow_date TEXT NOT NULL,"
        "    due_date TEXT NOT NULL,"
        "    return_date TEXT,"
        "    FOREIGN KEY(equipment_id) REFERENCES equipment(id) ON DELETE RESTRICT"
        ");"
        "CREATE INDEX IF NOT EXISTS idx_loans_equipment_id ON loans(equipment_id);"
        "CREATE INDEX IF NOT EXISTS idx_loans_student_roll ON loans(student_roll);"
        "CREATE INDEX IF NOT EXISTS idx_loans_return_date ON loans(return_date);";

    db_.execute(sql);
}

void LabManager::seedSampleDataIfEmpty() {
    Statement stmt(db_, "SELECT COUNT(*) FROM equipment;");
    if (stmt.step()) {
        int count = stmt.getInt(0);
        if (count == 0) {
            TransactionScope tx(db_);
            Statement insert(db_, "INSERT INTO equipment (name, category, total_quantity) VALUES (?, ?, ?);");
            
            std::vector<std::tuple<std::string, std::string, int>> samples = {
                {"Arduino Uno R3 Kit", "Embedded Systems", 5},
                {"Digital Storage Oscilloscope", "Electronics", 2},
                {"Raspberry Pi 4 Model B (4GB)", "IoT & Robotics", 4},
                {"Digital Multimeter (True-RMS)", "Test & Measurement", 6},
                {"FPGA Spartan-7 Board", "VLSI Design", 2}
            };

            for (const auto& item : samples) {
                insert.bindText(1, std::get<0>(item));
                insert.bindText(2, std::get<1>(item));
                insert.bindInt(3, std::get<2>(item));
                insert.execute();
            }
            tx.commit();
        }
    }
}

int LabManager::addEquipment(const std::string& name, const std::string& category, int quantity) {
    if (quantity <= 0) {
        throw DatabaseException("Quantity must be greater than zero.");
    }
    if (name.empty()) {
        throw DatabaseException("Equipment name cannot be empty.");
    }

    Statement stmt(db_, "INSERT INTO equipment (name, category, total_quantity) VALUES (?, ?, ?);");
    stmt.bindText(1, name);
    stmt.bindText(2, category.empty() ? "General" : category);
    stmt.bindInt(3, quantity);
    stmt.execute();

    return static_cast<int>(db_.lastInsertRowId());
}

std::vector<Equipment> LabManager::getAllEquipment() {
    std::vector<Equipment> list;
    const std::string sql =
        "SELECT e.id, e.name, e.category, e.total_quantity, "
        "       COALESCE(COUNT(l.id), 0) AS borrowed_count "
        "FROM equipment e "
        "LEFT JOIN loans l ON e.id = l.equipment_id AND l.return_date IS NULL "
        "GROUP BY e.id "
        "ORDER BY e.id ASC;";

    Statement stmt(db_, sql);
    while (stmt.step()) {
        Equipment eq;
        eq.id = stmt.getInt(0);
        eq.name = stmt.getText(1);
        eq.category = stmt.getText(2);
        eq.totalQuantity = stmt.getInt(3);
        eq.borrowedCount = stmt.getInt(4);
        eq.availableQuantity = eq.totalQuantity - eq.borrowedCount;
        if (eq.availableQuantity < 0) eq.availableQuantity = 0;
        list.push_back(eq);
    }
    return list;
}

std::optional<Equipment> LabManager::getEquipmentById(int equipmentId) {
    const std::string sql =
        "SELECT e.id, e.name, e.category, e.total_quantity, "
        "       COALESCE(COUNT(l.id), 0) AS borrowed_count "
        "FROM equipment e "
        "LEFT JOIN loans l ON e.id = l.equipment_id AND l.return_date IS NULL "
        "WHERE e.id = ? "
        "GROUP BY e.id;";

    Statement stmt(db_, sql);
    stmt.bindInt(1, equipmentId);
    if (stmt.step()) {
        Equipment eq;
        eq.id = stmt.getInt(0);
        eq.name = stmt.getText(1);
        eq.category = stmt.getText(2);
        eq.totalQuantity = stmt.getInt(3);
        eq.borrowedCount = stmt.getInt(4);
        eq.availableQuantity = eq.totalQuantity - eq.borrowedCount;
        if (eq.availableQuantity < 0) eq.availableQuantity = 0;
        return eq;
    }
    return std::nullopt;
}

int LabManager::issueEquipment(int equipmentId,
                              const std::string& studentRoll,
                              const std::string& studentName,
                              const std::string& department,
                              const std::string& dueDate) {
    if (studentRoll.empty() || studentName.empty()) {
        throw DatabaseException("Student Roll Number and Name are required.");
    }
    if (!isValidDate(dueDate)) {
        throw DatabaseException("Due date must be in YYYY-MM-DD format.");
    }

    TransactionScope tx(db_);

    // Check availability
    auto optEq = getEquipmentById(equipmentId);
    if (!optEq.has_value()) {
        throw DatabaseException("Equipment with ID #" + std::to_string(equipmentId) + " does not exist.");
    }

    if (optEq->availableQuantity <= 0) {
        throw DatabaseException("All units of '" + optEq->name + "' are currently on loan. (Available: 0)");
    }

    std::string today = getTodayDate();

    Statement stmt(db_,
        "INSERT INTO loans (equipment_id, student_roll, student_name, department, borrow_date, due_date, return_date) "
        "VALUES (?, ?, ?, ?, ?, ?, NULL);");
    stmt.bindInt(1, equipmentId);
    stmt.bindText(2, studentRoll);
    stmt.bindText(3, studentName);
    stmt.bindText(4, department.empty() ? "General" : department);
    stmt.bindText(5, today);
    stmt.bindText(6, dueDate);
    stmt.execute();

    int loanId = static_cast<int>(db_.lastInsertRowId());
    tx.commit();
    return loanId;
}

ReturnResult LabManager::returnEquipment(int loanId) {
    ReturnResult res;

    TransactionScope tx(db_);

    Statement check(db_,
        "SELECT l.id, l.return_date, l.student_roll, e.name "
        "FROM loans l "
        "JOIN equipment e ON l.equipment_id = e.id "
        "WHERE l.id = ?;");
    check.bindInt(1, loanId);

    if (!check.step()) {
        res.success = false;
        res.message = "No loan found with ID #" + std::to_string(loanId) + ".";
        return res;
    }

    res.equipmentName = check.getText(3);
    res.studentRoll = check.getText(2);

    if (!check.isNull(1)) {
        res.success = false;
        res.alreadyReturned = true;
        res.returnDate = check.getText(1);
        res.message = "Loan #" + std::to_string(loanId) + " was already returned on " + res.returnDate + ".";
        return res;
    }

    std::string today = getTodayDate();
    Statement update(db_, "UPDATE loans SET return_date = ? WHERE id = ?;");
    update.bindText(1, today);
    update.bindInt(2, loanId);
    update.execute();

    tx.commit();

    res.success = true;
    res.returnDate = today;
    res.message = "Successfully returned '" + res.equipmentName + "' (Student: " + res.studentRoll + ").";
    return res;
}

std::vector<LoanView> LabManager::getActiveLoans() {
    std::vector<LoanView> list;
    const std::string sql =
        "SELECT l.id, l.equipment_id, e.name, e.category, l.student_roll, l.student_name, "
        "       l.department, l.borrow_date, l.due_date, l.return_date "
        "FROM loans l "
        "JOIN equipment e ON l.equipment_id = e.id "
        "WHERE l.return_date IS NULL "
        "ORDER BY l.id ASC;";

    Statement stmt(db_, sql);
    std::string today = getTodayDate();

    while (stmt.step()) {
        LoanView lv;
        lv.loanId = stmt.getInt(0);
        lv.equipmentId = stmt.getInt(1);
        lv.equipmentName = stmt.getText(2);
        lv.category = stmt.getText(3);
        lv.studentRoll = stmt.getText(4);
        lv.studentName = stmt.getText(5);
        lv.department = stmt.getText(6);
        lv.borrowDate = stmt.getText(7);
        lv.dueDate = stmt.getText(8);
        lv.isReturned = false;
        lv.isOverdue = isOverdue(lv.dueDate, today);
        lv.daysDifference = daysBetween(today, lv.dueDate);
        list.push_back(lv);
    }
    return list;
}

std::vector<LoanView> LabManager::getOverdueLoans() {
    std::vector<LoanView> allActive = getActiveLoans();
    std::vector<LoanView> overdue;
    for (const auto& loan : allActive) {
        if (loan.isOverdue) {
            overdue.push_back(loan);
        }
    }
    return overdue;
}

std::vector<LoanView> LabManager::getAllLoans() {
    std::vector<LoanView> list;
    const std::string sql =
        "SELECT l.id, l.equipment_id, e.name, e.category, l.student_roll, l.student_name, "
        "       l.department, l.borrow_date, l.due_date, l.return_date "
        "FROM loans l "
        "JOIN equipment e ON l.equipment_id = e.id "
        "ORDER BY l.id DESC;";

    Statement stmt(db_, sql);
    std::string today = getTodayDate();

    while (stmt.step()) {
        LoanView lv;
        lv.loanId = stmt.getInt(0);
        lv.equipmentId = stmt.getInt(1);
        lv.equipmentName = stmt.getText(2);
        lv.category = stmt.getText(3);
        lv.studentRoll = stmt.getText(4);
        lv.studentName = stmt.getText(5);
        lv.department = stmt.getText(6);
        lv.borrowDate = stmt.getText(7);
        lv.dueDate = stmt.getText(8);
        if (!stmt.isNull(9)) {
            lv.returnDate = stmt.getText(9);
            lv.isReturned = true;
            lv.isOverdue = false;
        } else {
            lv.isReturned = false;
            lv.isOverdue = isOverdue(lv.dueDate, today);
            lv.daysDifference = daysBetween(today, lv.dueDate);
        }
        list.push_back(lv);
    }
    return list;
}

LabStats LabManager::getStatistics() {
    LabStats stats;

    Statement s1(db_, "SELECT COUNT(*), COALESCE(SUM(total_quantity), 0) FROM equipment;");
    if (s1.step()) {
        stats.totalEquipmentTypes = s1.getInt(0);
        stats.totalUnits = s1.getInt(1);
    }

    Statement s2(db_, "SELECT COUNT(*) FROM loans WHERE return_date IS NULL;");
    if (s2.step()) {
        stats.activeLoansCount = s2.getInt(0);
        stats.currentlyBorrowed = stats.activeLoansCount;
    }

    Statement s3(db_, "SELECT COUNT(*) FROM loans WHERE return_date IS NOT NULL;");
    if (s3.step()) {
        stats.returnedLoansCount = s3.getInt(0);
    }

    stats.availableUnits = stats.totalUnits - stats.currentlyBorrowed;
    if (stats.availableUnits < 0) stats.availableUnits = 0;

    auto overdue = getOverdueLoans();
    stats.overdueLoansCount = static_cast<int>(overdue.size());

    return stats;
}

// =================== Date Utilities ===================

std::string LabManager::getTodayDate() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::tm tm_buf;
#if defined(_WIN32) || defined(_WIN64)
    localtime_s(&tm_buf, &now_time);
#else
    localtime_r(&now_time, &tm_buf);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tm_buf, "%Y-%m-%d");
    return oss.str();
}

bool LabManager::isValidDate(const std::string& dateStr) {
    if (dateStr.length() != 10) return false;
    if (dateStr[4] != '-' || dateStr[7] != '-') return false;
    for (int i = 0; i < 10; ++i) {
        if (i == 4 || i == 7) continue;
        if (!std::isdigit(static_cast<unsigned char>(dateStr[i]))) return false;
    }

    int y = 0, m = 0, d = 0;
    if (sscanf(dateStr.c_str(), "%d-%d-%d", &y, &m, &d) != 3) return false;
    if (y < 2000 || y > 2100) return false;
    if (m < 1 || m > 12) return false;
    if (d < 1 || d > 31) return false;

    // Check days in month
    static const int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int maxDays = daysInMonth[m - 1];
    if (m == 2) {
        bool isLeap = (y % 4 == 0 && (y % 100 != 0 || y % 400 == 0));
        if (isLeap) maxDays = 29;
    }
    return d <= maxDays;
}

bool LabManager::isOverdue(const std::string& dueDate, const std::string& referenceDate) {
    // Both are YYYY-MM-DD. Lexicographical comparison works directly for ISO-8601 strings!
    return dueDate < referenceDate;
}

int LabManager::daysBetween(const std::string& fromDate, const std::string& toDate) {
    std::tm tm1 = {};
    std::tm tm2 = {};
    sscanf(fromDate.c_str(), "%d-%d-%d", &tm1.tm_year, &tm1.tm_mon, &tm1.tm_mday);
    sscanf(toDate.c_str(), "%d-%d-%d", &tm2.tm_year, &tm2.tm_mon, &tm2.tm_mday);

    tm1.tm_year -= 1900;
    tm1.tm_mon -= 1;
    tm2.tm_year -= 1900;
    tm2.tm_mon -= 1;

    std::time_t t1 = std::mktime(&tm1);
    std::time_t t2 = std::mktime(&tm2);
    if (t1 == -1 || t2 == -1) return 0;

    double diff = std::difftime(t2, t1);
    return static_cast<int>(diff / (60 * 60 * 24));
}

} // namespace lab
