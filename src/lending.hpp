#ifndef LENDING_HPP
#define LENDING_HPP

#include "database.hpp"
#include <string>
#include <vector>
#include <optional>

namespace lab {

struct Equipment {
    int id = 0;
    std::string name;
    std::string category;
    int totalQuantity = 0;
    int borrowedCount = 0;
    int availableQuantity = 0;
};

struct LoanView {
    int loanId = 0;
    int equipmentId = 0;
    std::string equipmentName;
    std::string category;
    std::string studentRoll;
    std::string studentName;
    std::string department;
    std::string borrowDate;
    std::string dueDate;
    std::string returnDate; // empty if not returned
    bool isReturned = false;
    bool isOverdue = false;
    int daysDifference = 0; // days overdue if overdue, or days remaining if active
};

struct ReturnResult {
    bool success = false;
    std::string message;
    std::string equipmentName;
    std::string studentRoll;
    std::string returnDate;
    bool alreadyReturned = false;
};

struct LabStats {
    int totalEquipmentTypes = 0;
    int totalUnits = 0;
    int currentlyBorrowed = 0;
    int availableUnits = 0;
    int activeLoansCount = 0;
    int overdueLoansCount = 0;
    int returnedLoansCount = 0;
};

class LabManager {
public:
    explicit LabManager(Database& db);

    // Schema and initialization
    void initializeSchema();
    void seedSampleDataIfEmpty();

    // Equipment operations
    int addEquipment(const std::string& name, const std::string& category, int quantity);
    std::vector<Equipment> getAllEquipment();
    std::optional<Equipment> getEquipmentById(int equipmentId);

    // Lending operations
    int issueEquipment(int equipmentId,
                       const std::string& studentRoll,
                       const std::string& studentName,
                       const std::string& department,
                       const std::string& dueDate);

    ReturnResult returnEquipment(int loanId);

    // Queries & Reports
    std::vector<LoanView> getActiveLoans();
    std::vector<LoanView> getOverdueLoans();
    std::vector<LoanView> getAllLoans();
    LabStats getStatistics();

    // Date utilities
    static std::string getTodayDate();
    static bool isValidDate(const std::string& dateStr); // YYYY-MM-DD
    static bool isOverdue(const std::string& dueDate, const std::string& referenceDate);
    static int daysBetween(const std::string& fromDate, const std::string& toDate);

private:
    Database& db_;
};

} // namespace lab

#endif // LENDING_HPP
