#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <string>
#include <stdexcept>
#include <memory>
#include <cstdint>

// Forward declaration of SQLite types
struct sqlite3;
struct sqlite3_stmt;

namespace lab {

class DatabaseException : public std::runtime_error {
public:
    explicit DatabaseException(const std::string& message, int errorCode = 0);
    int getErrorCode() const noexcept { return errorCode_; }

private:
    int errorCode_;
};

class Database {
public:
    explicit Database(const std::string& filepath);
    ~Database();

    // Non-copyable
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

    // Movable
    Database(Database&& other) noexcept;
    Database& operator=(Database&& other) noexcept;

    void execute(const std::string& sql);
    int64_t lastInsertRowId() const;
    int changes() const;

    void beginTransaction();
    void commitTransaction();
    void rollbackTransaction();

    sqlite3* handle() const noexcept { return db_; }

private:
    sqlite3* db_ = nullptr;
};

class Statement {
public:
    Statement(Database& db, const std::string& sql);
    ~Statement();

    // Non-copyable
    Statement(const Statement&) = delete;
    Statement& operator=(const Statement&) = delete;

    // Movable
    Statement(Statement&& other) noexcept;
    Statement& operator=(Statement&& other) noexcept;

    // Parameter binding (1-based index)
    void bindInt(int index, int val);
    void bindInt64(int index, int64_t val);
    void bindDouble(int index, double val);
    void bindText(int index, const std::string& val);
    void bindNull(int index);

    // Execution
    bool step();          // returns true if SQLITE_ROW, false if SQLITE_DONE
    void execute();       // runs to completion (for INSERT/UPDATE/DELETE)
    void reset();

    // Column extraction (0-based index)
    int getInt(int col) const;
    int64_t getInt64(int col) const;
    double getDouble(int col) const;
    std::string getText(int col) const;
    bool isNull(int col) const;

private:
    sqlite3_stmt* stmt_ = nullptr;
};

// RAII Transaction helper
class TransactionScope {
public:
    explicit TransactionScope(Database& db);
    ~TransactionScope();

    void commit();

    TransactionScope(const TransactionScope&) = delete;
    TransactionScope& operator=(const TransactionScope&) = delete;

private:
    Database& db_;
    bool committed_ = false;
};

} // namespace lab

#endif // DATABASE_HPP
