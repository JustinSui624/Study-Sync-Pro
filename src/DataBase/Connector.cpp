
#include <iostream>
#include <stdexcept>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/statement.h>
#include <cppconn/resultset.h>
#include <cppconn/prepared_statement.h>
#include <fstream>

void logSecurityEvent(const std::string& event, bool isError = false) {
    std::ofstream logFile("security_connector.log", std::ios::app);
    if (logFile.is_open()) {
        logFile << (isError ? "[ERROR] " : "[INFO] ") << event << std::endl;
        logFile.close();
    }
}

int main() {
    try {
        sql::mysql::MySQL_Driver* driver;
        sql::Connection* conn;

        driver = sql::mysql::get_mysql_driver_instance();
        
        conn = driver->connect("tcp://127.0.0.1:3306", "root", "1234");
        
        logSecurityEvent("Database connection attempt");

        conn->setSchema("@localhost");

        sql::PreparedStatement* pstmt = conn->prepareStatement(
            "SELECT Username, PasswordHash FROM UserLogin"
        );
        sql::ResultSet* res = pstmt->executeQuery();

        while (res->next()) {
            std::cout << "Username: " << res->getString("Username")
                      << ", PasswordHash: " << res->getString("PasswordHash") << std::endl;
        }
        
        logSecurityEvent("Query executed successfully");
        
        delete res;
        delete pstmt;
        delete conn;
        
    } catch (sql::SQLException& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        
        logSecurityEvent("SQL Error: " + std::string(e.what()), true);
    }

    return 0;
}
