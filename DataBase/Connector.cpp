#include <iostream>
#include <stdexcept>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/statement.h>
#include <cppconn/resultset.h>

int main() {
    try {
        sql::mysql::MySQL_Driver* driver;
        sql::Connection* conn;

        driver = sql::mysql::get_mysql_driver_instance();
        conn = driver->connect("tcp://127.0.0.1:3306", "root", "sun1day");

        conn->setSchema("@localhost");

        sql::Statement* stmt = conn->createStatement();
        sql::ResultSet* res = stmt->executeQuery("SELECT Username, PasswordHash FROM UserLogin");

        while (res->next()) {
            std::cout << "Username: " << res->getString("Username")
                      << ", PasswordHash: " << res->getString("PasswordHash") << std::endl;
        }
        delete res;
        delete stmt;
        delete conn;
    } catch (sql::SQLException& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}