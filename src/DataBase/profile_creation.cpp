#include <iostream>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/exception.h>
#include <sstream>
#include <regex>
#include <fstream>

namespace SecurityCheck {
    bool isValidUsername(const std::string& username) {
        if (username.find("'") != std::string::npos ||
            username.find("\"") != std::string::npos ||
            username.find(";") != std::string::npos ||
            username.find("--") != std::string::npos) {
            return false;
        }
        return username.length() >= 3 && username.length() <= 20;
    }
    
    bool isValidPassword(const std::string& password) {
        return password.length() >= 8;
    }
    
    void logEvent(const std::string& event) {
        std::ofstream log("security_profile.log", std::ios::app);
        if (log.is_open()) {
            log << event << std::endl;
            log.close();
        }
    }
}

int main() {
    std::string name, classes, username, password;
    int gradYear;

    std::cout << "Enter full name: ";
    std::getline(std::cin, name);
    std::cout << "Enter graduation year: ";
    std::cin >> gradYear;
    std::cin.ignore();
    std::cout << "Enter current classes (comma-separated): ";
    std::getline(std::cin, classes);
    std::cout << "Choose a username: ";
    std::getline(std::cin, username);
    std::cout << "Choose a password: ";
    std::getline(std::cin, password);

    if (!SecurityCheck::isValidUsername(username)) {
        std::cerr << "Invalid username. Please avoid special characters." << std::endl;
        SecurityCheck::logEvent("Invalid username attempt: " + username);
        return 1;
    }
    
    if (!SecurityCheck::isValidPassword(password)) {
        std::cerr << "Password must be at least 8 characters." << std::endl;
        SecurityCheck::logEvent("Weak password attempt");
        return 1;
    }

    try {
        sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
        
        std::unique_ptr<sql::Connection> conn(driver->connect("tcp://127.0.0.1:3306", "root", "1234"));
        conn->setSchema("@localhost");

        std::unique_ptr<sql::PreparedStatement> checkStmt(
            conn->prepareStatement("SELECT COUNT(*) as count FROM UserLogin WHERE Username = ?")
        );
        checkStmt->setString(1, username);
        std::unique_ptr<sql::ResultSet> checkRes(checkStmt->executeQuery());
        
        if (checkRes->next() && checkRes->getInt("count") > 0) {
            std::cerr << "Username already exists. Please choose another." << std::endl;
            SecurityCheck::logEvent("Duplicate username attempt: " + username);
            return 1;
        }

        std::unique_ptr<sql::PreparedStatement> stmt(
            conn->prepareStatement("INSERT INTO Profile (Name, GradYear, CurrentClasses) VALUES (?, ?, ?)")
        );
        stmt->setString(1, name);
        stmt->setInt(2, gradYear);
        stmt->setString(3, classes);
        stmt->executeUpdate();

        std::unique_ptr<sql::ResultSet> res(
            stmt->executeQuery("SELECT LAST_INSERT_ID() AS id")
        );
        int profileID = 0;
        if (res->next()) {
            profileID = res->getInt("id");
        }

        stmt.reset(conn->prepareStatement(
            "INSERT INTO UserLogin (Username, PasswordHash, ProfileID) VALUES (?, SHA2(?, 256), ?)"
        ));
        stmt->setString(1, username);
        stmt->setString(2, password);
        stmt->setInt(3, profileID);
        stmt->executeUpdate();

        std::cout << "✅ Profile and login created successfully!" << std::endl;
        
        SecurityCheck::logEvent("Profile created for user: " + username);
    } catch (sql::SQLException& e) {
        std::cerr << "SQL Error: " << e.what() << std::endl;
        SecurityCheck::logEvent("SQL Error during profile creation");
    }
    return 0;
}
