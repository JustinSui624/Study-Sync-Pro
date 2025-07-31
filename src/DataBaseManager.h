#pragma once
#include "pg_connector.hpp"
#include <string>
#include <vector>
#include <memory>

struct User {
    std::string username;
    std::string password;
    std::string name;
    int gradYear;
    std::string subjects;
    std::string location;
    int profileId = -1;
};

struct Group {
    std::string name;
    std::string description;
    std::string subjects;
    std::string location;
    int memberCount;
    int id = -1;
};

class DatabaseManager {
public:
    DatabaseManager(const std::string& connectionString);
    ~DatabaseManager() = default;
    
    // User operations
    bool createProfileAndLogin(const std::string& name, int gradYear, 
                              const std::string& subjects, const std::string& username, 
                              const std::string& password);
    bool authenticateUser(const std::string& username, const std::string& password, User& outUser);
    bool updateUserProfile(const User& user);
    
    // Group operations
    std::vector<Group> getAllGroups();
    std::vector<Group> getMatchingGroups(const std::vector<std::string>& subjects);
    bool createGroup(const Group& group);
    
    // Database initialization
    bool initializeTables();
    bool isConnected() const { return connected; }
    
private:
    std::string connString;
    bool connected;
    
    // Helper methods
    std::unique_ptr<PgConnector> getConnection();
    std::vector<std::string> parseSubjects(const std::string& subjects);
};