#include <iostream>
#include <string>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>

int main() {
    std::string username, password;

    std::cout << "=== Welcome to the Student Profile Login Page ===" << std::endl;
    std::cout << "Username: ";
    std::getline(std::cin, username);
    std::cout << "Password: ";
    std::getline(std::cin, password);

    try {
        sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
        std::unique_ptr<sql::Connection> conn(driver->connect("tcp://127.0.0.1:3306", "your_username", "your_password"));
        conn->setSchema("your_database_name");

        std::unique_ptr<sql::PreparedStatement> stmt(
            conn->prepareStatement("SELECT u.ProfileID, p.Name, p.GradYear, p.CurrentClasses "
                                   "FROM UserLogin u "
                                   "JOIN Profile p ON u.ProfileID = p.ID "
                                   "WHERE u.Username = ? AND u.PasswordHash = SHA2(?, 256)")
        );

        stmt->setString(1, username);
        stmt->setString(2, password);

        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery());

        if (res->next()) {
            std::cout << "\n✅ Login successful!" << std::endl;
            std::cout << "Welcome, " << res->getString("Name") << "!" << std::endl;
            std::cout << "Graduation Year: " << res->getInt("GradYear") << std::endl;
            std::cout << "Current Classes: " << res->getString("CurrentClasses") << std::endl;
        } else {
            std::cout << "\n❌ Login failed. Invalid username or password." << std::endl;
        }

    } catch (sql::SQLException& e) {
        std::cerr << "Database Error: " << e.what() << std::endl;
    }

    return 0;
}