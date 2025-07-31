#include <iostream>
#include <string>
#include <libpq-fe.h>

void joinGroupByUsername(const std::string& username, const std::string& groupName) {
    PgConnector db("postgresql://postgres:cen3031group4@db.iekosjtwireodvbaqhcm.supabase.co:5432/postgres");

    if (PQstatus(conn) != CONNECTION_OK) {
        std::cerr << "Connection failed: " << PQerrorMessage(conn);
        PQfinish(conn);
        return;
    }

    // Get profile_id from login
    std::string profileQuery = "SELECT profile_id FROM login WHERE username = '" + username + "';";
    PGresult* res = PQexec(conn, profileQuery.c_str());

    if (PQresultStatus(res) != PGRES_TUPLES_OK || PQntuples(res) == 0) {
        std::cerr << "User not found.\n";
        PQclear(res);
        PQfinish(conn);
        return;
    }

    std::string profile_id = PQgetvalue(res, 0, 0);
    PQclear(res);

    // Get group_id from group name
    std::string groupQuery = "SELECT id FROM \"group\" WHERE name = '" + groupName + "';";
    res = PQexec(conn, groupQuery.c_str());

    if (PQresultStatus(res) != PGRES_TUPLES_OK || PQntuples(res) == 0) {
        std::cerr << "Group not found.\n";
        PQclear(res);
        PQfinish(conn);
        return;
    }

    std::string group_id = PQgetvalue(res, 0, 0);
    PQclear(res);

    // Insert into group_membership
    std::string insertQuery =
        "INSERT INTO group_membership (profile_id, group_id) VALUES (" + profile_id + ", " + group_id + ");";

    res = PQexec(conn, insertQuery.c_str());

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        std::cerr << "Failed to join group: " << PQerrorMessage(conn);
        PQclear(res);
        PQfinish(conn);
        return;
    }

    PQclear(res);
    PQfinish(conn);
    std::cout << "✅ User '" << username << "' successfully joined group '" << groupName << "'!\n";
}

int main() {
    //example
    std::string username = "emma_j";
    std::string groupName = "Math Study Buddies";

    joinGroupByUsername(username, groupName);

    return 0;
}