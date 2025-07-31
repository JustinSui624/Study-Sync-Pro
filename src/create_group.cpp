#include <iostream>
#include <string>
#include <libpq-fe.h>

void createGroup(const std::string& groupName, const std::string& description) {
    PgConnector db("postgresql://postgres:cen3031group4@db.iekosjtwireodvbaqhcm.supabase.co:5432/postgres");

    if (PQstatus(conn) != CONNECTION_OK) {
        std::cerr << "Connection failed: " << PQerrorMessage(conn);
        PQfinish(conn);
        return;
    }

    // Check if group already exists
    std::string checkQuery = "SELECT id FROM \"group\" WHERE name = '" + groupName + "';";
    PGresult* res = PQexec(conn, checkQuery.c_str());

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "Failed to check for existing group.\n";
        PQclear(res);
        PQfinish(conn);
        return;
    }

    if (PQntuples(res) > 0) {
        std::cout << "⚠️ Group already exists.\n";
        PQclear(res);
        PQfinish(conn);
        return;
    }
    PQclear(res);

    // Insert new group
    std::string insertQuery =
        "INSERT INTO \"group\" (name, description) VALUES ('" + groupName + "', '" + description + "');";

    res = PQexec(conn, insertQuery.c_str());

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        std::cerr << "Failed to create group: " << PQerrorMessage(conn);
        PQclear(res);
        PQfinish(conn);
        return;
    }

    std::cout << "✅ Group '" << groupName << "' created successfully!\n";
    PQclear(res);
    PQfinish(conn);
}

int main() {
    //example
    std::string groupName = "CS Project Team";
    std::string description = "A group for collaborating on our final computer science project.";

    createGroup(groupName, description);
    return 0;
}