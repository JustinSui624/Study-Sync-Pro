#include "pg_connector.hpp"
#include <iostream>

int main() {
    try {
        PgConnector db("postgresql://postgres:cen3031group4@db.iekosjtwireodvbaqhcm.supabase.co:5432/postgres");

        PGresult* res = db.exec("SELECT version()");
        std::cout << "PostgreSQL version: " << PQgetvalue(res, 0, 0) << "\n";
        PQclear(res);

        std::vector<const char*> params = { "5", "7" };
        PGresult* res2 = db.execParams("SELECT $1::int + $2::int AS sum", params);
        std::cout << "Sum: " << PQgetvalue(res2, 0, 0) << "\n";
        PQclear(res2);
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}