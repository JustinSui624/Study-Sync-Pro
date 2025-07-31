#pragma once
#include <libpq-fe.h>
#include <stdexcept>
#include <string>
#include <vector>

class PgConnector {
public:
    explicit PgConnector(const std::string& conninfo) {
        conn_ = PQconnectdb(conninfo.c_str());
        if (PQstatus(conn_) != CONNECTION_OK) {
            std::string err = PQerrorMessage(conn_);
            PQfinish(conn_);
            throw std::runtime_error("Connection failed: " + err);
        }
    }

    ~PgConnector() {
        if (conn_) PQfinish(conn_);
    }

    // Prevent copy
    PgConnector(const PgConnector&) = delete;
    PgConnector& operator=(const PgConnector&) = delete;

    // Allow move
    PgConnector(PgConnector&& other) noexcept : conn_(other.conn_) {
        other.conn_ = nullptr;
    }
    PgConnector& operator=(PgConnector&& other) noexcept {
        if (this != &other) {
            if (conn_) PQfinish(conn_);
            conn_ = other.conn_;
            other.conn_ = nullptr;
        }
        return *this;
    }

    PGresult* exec(const std::string& sql) {
        PGresult* res = PQexec(conn_, sql.c_str());
        checkResult(res);
        return res;
    }

    PGresult* execParams(const std::string& sql, const std::vector<const char*>& params) {
        PGresult* res = PQexecParams(conn_,
                                     sql.c_str(),
                                     static_cast<int>(params.size()),
                                     nullptr, params.data(),
                                     nullptr, nullptr, 0);
        checkResult(res);
        return res;
    }

private:
    void checkResult(PGresult* res) {
        if (PQresultStatus(res) != PGRES_TUPLES_OK &&
            PQresultStatus(res) != PGRES_COMMAND_OK) {
            std::string err = PQerrorMessage(conn_);
            PQclear(res);
            throw std::runtime_error("Query failed: " + err);
        }
    }

    PGconn* conn_ = nullptr;
};