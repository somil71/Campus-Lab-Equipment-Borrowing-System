#include "database.hpp"
#include "sqlite3.h"
#include <sstream>
#include <utility>

namespace lab {

DatabaseException::DatabaseException(const std::string& message, int errorCode)
    : std::runtime_error(message), errorCode_(errorCode) {}

// =================== Database ===================

Database::Database(const std::string& filepath) {
    int rc = sqlite3_open(filepath.c_str(), &db_);
    if (rc != SQLITE_OK) {
        std::string err = db_ ? sqlite3_errmsg(db_) : "Unable to allocate memory for SQLite";
        if (db_) {
            sqlite3_close(db_);
            db_ = nullptr;
        }
        throw DatabaseException("Cannot open database: " + err, rc);
    }

    // Enable foreign keys and WAL mode for better concurrency
    execute("PRAGMA foreign_keys = ON;");
}

Database::~Database() {
    if (db_) {
        sqlite3_close(db_);
        db_ = nullptr;
    }
}

Database::Database(Database&& other) noexcept : db_(other.db_) {
    other.db_ = nullptr;
}

Database& Database::operator=(Database&& other) noexcept {
    if (this != &other) {
        if (db_) {
            sqlite3_close(db_);
        }
        db_ = other.db_;
        other.db_ = nullptr;
    }
    return *this;
}

void Database::execute(const std::string& sql) {
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::string error = errMsg ? errMsg : "Unknown SQL error";
        sqlite3_free(errMsg);
        throw DatabaseException("SQL execution error: " + error, rc);
    }
}

int64_t Database::lastInsertRowId() const {
    return sqlite3_last_insert_rowid(db_);
}

int Database::changes() const {
    return sqlite3_changes(db_);
}

void Database::beginTransaction() {
    execute("BEGIN TRANSACTION;");
}

void Database::commitTransaction() {
    execute("COMMIT;");
}

void Database::rollbackTransaction() {
    execute("ROLLBACK;");
}

// =================== Statement ===================

Statement::Statement(Database& db, const std::string& sql) {
    int rc = sqlite3_prepare_v2(db.handle(), sql.c_str(), -1, &stmt_, nullptr);
    if (rc != SQLITE_OK) {
        std::string err = sqlite3_errmsg(db.handle());
        throw DatabaseException("Prepare failed: " + err + " (SQL: " + sql + ")", rc);
    }
}

Statement::~Statement() {
    if (stmt_) {
        sqlite3_finalize(stmt_);
        stmt_ = nullptr;
    }
}

Statement::Statement(Statement&& other) noexcept : stmt_(other.stmt_) {
    other.stmt_ = nullptr;
}

Statement& Statement::operator=(Statement&& other) noexcept {
    if (this != &other) {
        if (stmt_) {
            sqlite3_finalize(stmt_);
        }
        stmt_ = other.stmt_;
        other.stmt_ = nullptr;
    }
    return *this;
}

void Statement::bindInt(int index, int val) {
    int rc = sqlite3_bind_int(stmt_, index, val);
    if (rc != SQLITE_OK) throw DatabaseException("Bind int error", rc);
}

void Statement::bindInt64(int index, int64_t val) {
    int rc = sqlite3_bind_int64(stmt_, index, val);
    if (rc != SQLITE_OK) throw DatabaseException("Bind int64 error", rc);
}

void Statement::bindDouble(int index, double val) {
    int rc = sqlite3_bind_double(stmt_, index, val);
    if (rc != SQLITE_OK) throw DatabaseException("Bind double error", rc);
}

void Statement::bindText(int index, const std::string& val) {
    int rc = sqlite3_bind_text(stmt_, index, val.c_str(), -1, SQLITE_TRANSIENT);
    if (rc != SQLITE_OK) throw DatabaseException("Bind text error", rc);
}

void Statement::bindNull(int index) {
    int rc = sqlite3_bind_null(stmt_, index);
    if (rc != SQLITE_OK) throw DatabaseException("Bind null error", rc);
}

bool Statement::step() {
    int rc = sqlite3_step(stmt_);
    if (rc == SQLITE_ROW) return true;
    if (rc == SQLITE_DONE) return false;
    throw DatabaseException("Statement step error: " + std::to_string(rc), rc);
}

void Statement::execute() {
    int rc = sqlite3_step(stmt_);
    if (rc != SQLITE_DONE && rc != SQLITE_ROW) {
        throw DatabaseException("Statement execute error: " + std::to_string(rc), rc);
    }
    reset();
}

void Statement::reset() {
    sqlite3_reset(stmt_);
    sqlite3_clear_bindings(stmt_);
}

int Statement::getInt(int col) const {
    return sqlite3_column_int(stmt_, col);
}

int64_t Statement::getInt64(int col) const {
    return sqlite3_column_int64(stmt_, col);
}

double Statement::getDouble(int col) const {
    return sqlite3_column_double(stmt_, col);
}

std::string Statement::getText(int col) const {
    const unsigned char* text = sqlite3_column_text(stmt_, col);
    return text ? reinterpret_cast<const char*>(text) : "";
}

bool Statement::isNull(int col) const {
    return sqlite3_column_type(stmt_, col) == SQLITE_NULL;
}

// =================== TransactionScope ===================

TransactionScope::TransactionScope(Database& db) : db_(db) {
    db_.beginTransaction();
}

TransactionScope::~TransactionScope() {
    if (!committed_) {
        try {
            db_.rollbackTransaction();
        } catch (...) {
            // Suppress exception in destructor
        }
    }
}

void TransactionScope::commit() {
    if (!committed_) {
        db_.commitTransaction();
        committed_ = true;
    }
}

} // namespace lab
