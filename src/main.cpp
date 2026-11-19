#include "database.hpp"
#include "lending.hpp"

#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <limits>

using namespace lab;

// Utility functions for UI formatting
namespace ui {

void printHeader(const std::string& title) {
    std::cout << "\n========================================================================\n";
    std::cout << "  " << title << "\n";
    std::cout << "========================================================================\n";
}

void printDivider() {
    std::cout << "------------------------------------------------------------------------\n";
}

void clearCin() {
    std::cin.clear();
    std::string dummy;
    std::getline(std::cin, dummy);
}

std::string readLine(const std::string& prompt) {
    std::cout << prompt;
    std::string line;
    if (!std::getline(std::cin, line)) {
        return "";
    }
    // Trim leading and trailing whitespace
    size_t first = line.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = line.find_last_not_of(" \t\r\n");
    return line.substr(first, (last - first + 1));
}

int readInt(const std::string& prompt, int minVal = 0, int maxVal = 100000) {
    while (true) {
        std::cout << prompt;
        int val;
        if (std::cin >> val) {
            clearCin();
            if (val >= minVal && val <= maxVal) {
                return val;
            }
            std::cout << " [!] Please enter a number between " << minVal << " and " << maxVal << ".\n";
        } else {
            if (std::cin.eof()) {
                return -1;
            }
            clearCin();
            std::cout << " [!] Invalid input. Please enter an integer.\n";
        }
    }
}

} // namespace ui

void showAllEquipment(LabManager& mgr) {
    ui::printHeader("LAB INVENTORY & AVAILABILITY STATUS");
    auto list = mgr.getAllEquipment();

    if (list.empty()) {
        std::cout << "  No equipment records found in the lab database.\n";
        return;
    }

    std::cout << std::left
              << std::setw(6)  << "ID"
              << std::setw(32) << "Equipment Name"
              << std::setw(20) << "Category"
              << std::setw(8)  << "Total"
              << std::setw(10) << "Borrowed"
              << std::setw(10) << "Available"
              << "\n";
    ui::printDivider();

    for (const auto& eq : list) {
        std::cout << std::left
                  << std::setw(6)  << eq.id
                  << std::setw(32) << (eq.name.length() > 30 ? eq.name.substr(0, 27) + "..." : eq.name)
                  << std::setw(20) << (eq.category.length() > 18 ? eq.category.substr(0, 15) + "..." : eq.category)
                  << std::setw(8)  << eq.totalQuantity
                  << std::setw(10) << eq.borrowedCount
                  << std::setw(10) << eq.availableQuantity
                  << "\n";
    }
    ui::printDivider();
}

void addNewEquipment(LabManager& mgr) {
    ui::printHeader("ADD NEW LAB EQUIPMENT");
    std::string name = ui::readLine("  Enter Equipment Name : ");
    if (name.empty()) {
        std::cout << "  [x] Equipment name cannot be empty.\n";
        return;
    }

    std::string category = ui::readLine("  Enter Category       : ");
    if (category.empty()) category = "General";

    int quantity = ui::readInt("  Enter Total Quantity : ", 1, 10000);

    try {
        int id = mgr.addEquipment(name, category, quantity);
        std::cout << "\n  [✓] Success: Added '" << name << "' (ID: #" << id 
                  << ") with stock of " << quantity << " units.\n";
    } catch (const std::exception& ex) {
        std::cout << "\n  [x] Database Error: " << ex.what() << "\n";
    }
}

void issueEquipment(LabManager& mgr) {
    ui::printHeader("ISSUE EQUIPMENT TO STUDENT");
    showAllEquipment(mgr);

    int eqId = ui::readInt("\n  Enter Equipment ID to Borrow: ", 1, 999999);
    auto optEq = mgr.getEquipmentById(eqId);
    if (!optEq.has_value()) {
        std::cout << "  [x] Error: Equipment ID #" << eqId << " does not exist.\n";
        return;
    }

    if (optEq->availableQuantity <= 0) {
        std::cout << "  [!] Cannot issue: All " << optEq->totalQuantity 
                  << " units of '" << optEq->name << "' are currently borrowed!\n";
        return;
    }

    std::cout << "  Selected: " << optEq->name << " (Available: " << optEq->availableQuantity << ")\n";

    std::string roll = ui::readLine("  Student Roll / ID Number : ");
    if (roll.empty()) {
        std::cout << "  [x] Student Roll Number cannot be empty.\n";
        return;
    }

    std::string studentName = ui::readLine("  Student Full Name        : ");
    if (studentName.empty()) studentName = "Student " + roll;

    std::string dept = ui::readLine("  Department / Class       : ");
    if (dept.empty()) dept = "ECE / CSE";

    std::string today = LabManager::getTodayDate();
    std::cout << "  Today's Date: " << today << "\n";

    std::string dueDate = ui::readLine("  Due Date (YYYY-MM-DD)    : ");
    if (dueDate.empty()) {
        // Default to today + 7 days
        dueDate = today;
    }

    if (!LabManager::isValidDate(dueDate)) {
        std::cout << "  [x] Invalid date format. Must be YYYY-MM-DD (e.g., 2026-11-26).\n";
        return;
    }

    try {
        int loanId = mgr.issueEquipment(eqId, roll, studentName, dept, dueDate);
        std::cout << "\n  [✓] LOAN CONFIRMED!\n";
        std::cout << "      Loan ID      : #" << loanId << "\n";
        std::cout << "      Equipment    : " << optEq->name << "\n";
        std::cout << "      Student      : " << studentName << " (" << roll << ")\n";
        std::cout << "      Department   : " << dept << "\n";
        std::cout << "      Borrow Date  : " << today << "\n";
        std::cout << "      Due Date     : " << dueDate << "\n";
        std::cout << "      Remaining    : " << (optEq->availableQuantity - 1) << " unit(s) in lab.\n";
    } catch (const std::exception& ex) {
        std::cout << "\n  [x] Issue Failed: " << ex.what() << "\n";
    }
}

void returnEquipment(LabManager& mgr) {
    ui::printHeader("RETURN BORROWED EQUIPMENT");
    int loanId = ui::readInt("  Enter Loan ID to Return: ", 1, 999999);

    ReturnResult res = mgr.returnEquipment(loanId);
    if (res.success) {
        std::cout << "\n  [✓] RETURN COMPLETED!\n";
        std::cout << "      " << res.message << "\n";
        std::cout << "      Date of Return: " << res.returnDate << "\n";
        std::cout << "      Equipment stock has been restored to inventory.\n";
    } else {
        if (res.alreadyReturned) {
            std::cout << "\n  [!] ALREADY RETURNED: " << res.message << "\n";
        } else {
            std::cout << "\n  [x] Return Failed: " << res.message << "\n";
        }
    }
}

void viewActiveAndOverdueLoans(LabManager& mgr) {
    ui::printHeader("CURRENT ACTIVE & OVERDUE LOANS");
    auto activeLoans = mgr.getActiveLoans();

    if (activeLoans.empty()) {
        std::cout << "  No active loans. All equipment is safely stored in the lab!\n";
        return;
    }

    std::cout << std::left
              << std::setw(6)  << "Loan"
              << std::setw(12) << "Roll No"
              << std::setw(18) << "Student"
              << std::setw(24) << "Equipment"
              << std::setw(12) << "Due Date"
              << std::setw(14) << "Status"
              << "\n";
    ui::printDivider();

    int overdueCount = 0;
    for (const auto& loan : activeLoans) {
        std::string statusStr = "ACTIVE";
        if (loan.isOverdue) {
            statusStr = "OVERDUE (" + std::to_string(std::abs(loan.daysDifference)) + "d)";
            overdueCount++;
        }

        std::cout << std::left
                  << std::setw(6)  << ("#" + std::to_string(loan.loanId))
                  << std::setw(12) << loan.studentRoll
                  << std::setw(18) << (loan.studentName.length() > 16 ? loan.studentName.substr(0, 14) + ".." : loan.studentName)
                  << std::setw(24) << (loan.equipmentName.length() > 22 ? loan.equipmentName.substr(0, 20) + ".." : loan.equipmentName)
                  << std::setw(12) << loan.dueDate
                  << std::setw(14) << statusStr
                  << "\n";
    }
    ui::printDivider();
    std::cout << "  Total Active Loans: " << activeLoans.size()
              << " | Overdue: " << overdueCount << "\n";
}

void viewLabStatistics(LabManager& mgr) {
    ui::printHeader("LAB AUDIT & STATISTICAL SUMMARY");
    LabStats stats = mgr.getStatistics();

    std::cout << "  • Unique Equipment Catalog Types : " << stats.totalEquipmentTypes << "\n";
    std::cout << "  • Total Physical Units in Lab   : " << stats.totalUnits << "\n";
    std::cout << "  • Units Currently Available     : " << stats.availableUnits << "\n";
    std::cout << "  • Units Currently on Loan       : " << stats.currentlyBorrowed << "\n";
    std::cout << "  • Active Student Loans          : " << stats.activeLoansCount << "\n";
    std::cout << "  • Overdue Loans (Action Req.)   : " << stats.overdueLoansCount << "\n";
    std::cout << "  • Total Completed Returns       : " << stats.returnedLoansCount << "\n";
    ui::printDivider();
}

int main(int argc, char* argv[]) {
    std::string dbPath = "lab_equipment.db";

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--db" && i + 1 < argc) {
            dbPath = argv[++i];
        }
    }

    std::cout << "\n========================================================================\n";
    std::cout << "    CAMPUS LAB EQUIPMENT BORROWING SYSTEM (C++17 + SQLite3)\n";
    std::cout << "========================================================================\n";
    std::cout << "  Connecting to Lab Database: " << dbPath << " ...\n";

    try {
        Database db(dbPath);
        LabManager mgr(db);
        std::cout << "  [✓] Database Connected & Schema Verified Successfully!\n";

        bool running = true;
        while (running) {
            std::cout << "\n======================== MAIN MENU ========================\n";
            std::cout << "  1. Add Equipment to Inventory\n";
            std::cout << "  2. View Equipment & Stock Availability\n";
            std::cout << "  3. Issue Equipment to Student (Borrow)\n";
            std::cout << "  4. Return Equipment\n";
            std::cout << "  5. View Active & Overdue Loans\n";
            std::cout << "  6. Lab Statistics & Audit Summary\n";
            std::cout << "  7. Exit System\n";
            std::cout << "===========================================================\n";

            int choice = ui::readInt("  Select an option [1-7]: ", 1, 7);
            if (choice == -1) {
                running = false;
                break;
            }

            switch (choice) {
                case 1:
                    addNewEquipment(mgr);
                    break;
                case 2:
                    showAllEquipment(mgr);
                    break;
                case 3:
                    issueEquipment(mgr);
                    break;
                case 4:
                    returnEquipment(mgr);
                    break;
                case 5:
                    viewActiveAndOverdueLoans(mgr);
                    break;
                case 6:
                    viewLabStatistics(mgr);
                    break;
                case 7:
                    std::cout << "\n  Closing database connection safely. Goodbye!\n";
                    running = false;
                    break;
                default:
                    std::cout << "  [!] Unknown option.\n";
                    break;
            }
        }
    } catch (const std::exception& ex) {
        std::cerr << "\n  [FATAL ERROR] " << ex.what() << "\n";
        return 1;
    }

    return 0;
}
