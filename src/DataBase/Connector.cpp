#include "pg_connector.hpp"
#include <iostream>
#include <string>

int main() {
    std::string name = "Emma Johnson";
    int gradYear = 2026;
    std::string subjects = "Math, Biology";
    std::string username = "emma_j";
    std::string password = "securepass123";

    createProfileAndLogin(name, gradYear, subjects, username, password);

    return 0;
}

void createProfileAndLogin(
    const std::string& name,
    int gradYear,
    const std::string& subjects,
    const std::string& username,
    const std::string& password
) {
    // Connect to database
    PgConnector db("postgresql://postgres:cen3031group4@db.iekosjtwireodvbaqhcm.supabase.co:5432/postgres");

    if (PQstatus(conn) != CONNECTION_OK) {
        std::cerr << "Connection failed: " << PQerrorMessage(conn);
        PQfinish(conn);
        return;
    }

    // Begin transaction
    PGresult* res = PQexec(conn, "BEGIN");
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        std::cerr << "BEGIN failed\n";
        PQclear(res);
        PQfinish(conn);
        return;
    }
    PQclear(res);

    // Insert into profile
    std::string profileQuery =
        "INSERT INTO profile (name, grad_year, subjects) "
        "VALUES ('" + name + "', " + std::to_string(gradYear) + ", '" + subjects + "') RETURNING id;";

    res = PQexec(conn, profileQuery.c_str());

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "Profile insertion failed: " << PQerrorMessage(conn);
        PQclear(res);
        PQexec(conn, "ROLLBACK");
        PQfinish(conn);
        return;
    }

    // Get the returned profile_id
    std::string profile_id = PQgetvalue(res, 0, 0);
    PQclear(res);

    // Insert into login
    std::string loginQuery =
        "INSERT INTO login (username, password, profile_id) "
        "VALUES ('" + username + "', '" + password + "', " + profile_id + ");";

    res = PQexec(conn, loginQuery.c_str());

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        std::cerr << "Login insertion failed: " << PQerrorMessage(conn);
        PQclear(res);
        PQexec(conn, "ROLLBACK");
        PQfinish(conn);
        return;
    }
    PQclear(res);

    PQexec(conn, "COMMIT");
    PQfinish(conn);

    std::cout << "✅ Profile and login created successfully!\n";
}