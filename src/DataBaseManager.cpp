#include "DatabaseManager.h"
#include <iostream>
#include <sstream>
#include <algorithm>

DatabaseManager::DatabaseManager(const std::string& connectionString) 
    : connString(connectionString), connected(false) {
    try {
        auto conn = getConnection();
        connected = true;
        std::cout << "✅ Connected to PostgreSQL database successfully!" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "❌ Failed to connect to database: " << e.what() << std::endl;
        connected = false;
    }
}

std::unique_ptr<PgConnector> DatabaseManager::getConnection() {
    return std::make_unique<PgConnector>(connString);
}

std::vector<std::string> DatabaseManager::parseSubjects(const std::string& subjects) {
    std::vector<std::string> result;
    std::stringstream ss(subjects);
    std::string subject;
    
    while (std::getline(ss, subject, ',')) {
        // Trim whitespace
        subject.erase(0, subject.find_first_not_of(" \t"));
        subject.erase(subject.find_last_not_of(" \t") + 1);
        if (!subject.empty()) {
            result.push_back(subject);
        }
    }
    return result;
}

bool DatabaseManager::initializeTables() {
    if (!connected) return false;
    
    try {
        auto conn = getConnection();
        
        // Create profile table
        std::string createProfileTable = R"(
            CREATE TABLE IF NOT EXISTS profile (
                id SERIAL PRIMARY KEY,
                name VARCHAR(100) NOT NULL,
                grad_year INTEGER,
                subjects TEXT,
                location VARCHAR(100),
                created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
            );
        )";
        
        // Create login table
        std::string createLoginTable = R"(
            CREATE TABLE IF NOT EXISTS login (
                id SERIAL PRIMARY KEY,
                username VARCHAR(50) UNIQUE NOT NULL,
                password VARCHAR(255) NOT NULL,
                profile_id INTEGER REFERENCES profile(id) ON DELETE CASCADE,
                created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
            );
        )";
        
        // Create groups table
        std::string createGroupsTable = R"(
            CREATE TABLE IF NOT EXISTS study_groups (
                id SERIAL PRIMARY KEY,
                name VARCHAR(200) NOT NULL,
                description TEXT,
                subjects TEXT NOT NULL,
                location VARCHAR(100),
                member_count INTEGER DEFAULT 0,
                created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
            );
        )";
        
        PGresult* res1 = conn->exec(createProfileTable);
        PQclear(res1);
        
        PGresult* res2 = conn->exec(createLoginTable);
        PQclear(res2);
        
        PGresult* res3 = conn->exec(createGroupsTable);
        PQclear(res3);
        
        // Insert sample groups if table is empty
        std::string checkGroups = "SELECT COUNT(*) FROM study_groups;";
        PGresult* countRes = conn->exec(checkGroups);
        int groupCount = std::stoi(PQgetvalue(countRes, 0, 0));
        PQclear(countRes);
        
        if (groupCount == 0) {
            std::string insertGroups = R"(
                INSERT INTO study_groups (name, description, subjects, location, member_count) VALUES
                ('Advanced Calculus Study Circle', 'Tackle multivariable calculus and differential equations', 'calculus,mathematics,derivatives,integrals', 'MIT', 8),
                ('Linear Algebra Workshop', 'Matrix operations and vector spaces', 'linear algebra,mathematics,matrices,vectors', 'Stanford University', 12),
                ('Organic Chemistry Lab', 'Synthesis and reaction mechanisms', 'chemistry,organic chemistry,lab,reactions', 'Harvard University', 10),
                ('Physics Problem Solvers', 'Classical and quantum mechanics', 'physics,mechanics,quantum,thermodynamics', 'Caltech', 7),
                ('Data Structures & Algorithms', 'Coding interview prep and theory', 'computer science,algorithms,programming,java,python', 'UC Berkeley', 15),
                ('Biology Study Group', 'Molecular and cell biology', 'biology,genetics,molecular biology,cell biology', 'Yale University', 9),
                ('Statistics & Data Analysis', 'Statistical modeling and analysis', 'statistics,data science,mathematics,probability', 'University of Chicago', 11),
                ('Engineering Mechanics', 'Statics and dynamics problems', 'engineering,mechanics,statics,dynamics', 'Georgia Tech', 13);
            )";
            PGresult* insertRes = conn->exec(insertGroups);
            PQclear(insertRes);
            std::cout << "✅ Sample study groups inserted!" << std::endl;
        }
        
        std::cout << "✅ Database tables initialized successfully!" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cout << "❌ Failed to initialize tables: " << e.what() << std::endl;
        return false;
    }
}

bool DatabaseManager::createProfileAndLogin(const std::string& name, int gradYear, 
                                           const std::string& subjects, const std::string& username, 
                                           const std::string& password) {
    if (!connected) return false;
    
    try {
        auto conn = getConnection();
        
        // Begin transaction
        PGresult* beginRes = conn->exec("BEGIN");
        PQclear(beginRes);
        
        // Check if username already exists
        std::vector<const char*> checkParams = {username.c_str()};
        PGresult* checkRes = conn->execParams(
            "SELECT username FROM login WHERE username = $1", 
            checkParams
        );
        
        if (PQntuples(checkRes) > 0) {
            PQclear(checkRes);
            PGresult* rollbackRes = conn->exec("ROLLBACK");
            PQclear(rollbackRes);
            std::cout << "❌ Username already exists!" << std::endl;
            return false;
        }
        PQclear(checkRes);
        
        // Insert into profile
        std::vector<const char*> profileParams = {
            name.c_str(),
            std::to_string(gradYear).c_str(),
            subjects.c_str()
        };
        
        PGresult* profileRes = conn->execParams(
            "INSERT INTO profile (name, grad_year, subjects) VALUES ($1, $2, $3) RETURNING id",
            profileParams
        );
        
        if (PQntuples(profileRes) == 0) {
            PQclear(profileRes);
            PGresult* rollbackRes = conn->exec("ROLLBACK");
            PQclear(rollbackRes);
            return false;
        }
        
        std::string profileId = PQgetvalue(profileRes, 0, 0);
        PQclear(profileRes);
        
        // Insert into login
        std::vector<const char*> loginParams = {
            username.c_str(),
            password.c_str(),
            profileId.c_str()
        };
        
        PGresult* loginRes = conn->execParams(
            "INSERT INTO login (username, password, profile_id) VALUES ($1, $2, $3)",
            loginParams
        );
        PQclear(loginRes);
        
        // Commit transaction
        PGresult* commitRes = conn->exec("COMMIT");
        PQclear(commitRes);
        
        std::cout << "✅ Profile and login created successfully!" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cout << "❌ Registration failed: " << e.what() << std::endl;
        return false;
    }
}

bool DatabaseManager::authenticateUser(const std::string& username, const std::string& password, User& outUser) {
    if (!connected) return false;
    
    try {
        auto conn = getConnection();
        
        std::vector<const char*> params = {username.c_str(), password.c_str()};
        PGresult* res = conn->execParams(
            "SELECT l.username, l.password, p.name, p.grad_year, p.subjects, p.location, p.id "
            "FROM login l JOIN profile p ON l.profile_id = p.id "
            "WHERE l.username = $1 AND l.password = $2",
            params
        );
        
        if (PQntuples(res) > 0) {
            outUser.username = PQgetvalue(res, 0, 0);
            outUser.password = PQgetvalue(res, 0, 1);
            outUser.name = PQgetvalue(res, 0, 2);
            outUser.gradYear = std::stoi(PQgetvalue(res, 0, 3));
            outUser.subjects = PQgetvalue(res, 0, 4) ? PQgetvalue(res, 0, 4) : "";
            outUser.location = PQgetvalue(res, 0, 5) ? PQgetvalue(res, 0, 5) : "";
            outUser.profileId = std::stoi(PQgetvalue(res, 0, 6));
            
            PQclear(res);
            std::cout << "✅ Login successful for " << username << std::endl;
            return true;
        }
        
        PQclear(res);
        std::cout << "❌ Invalid credentials!" << std::endl;
        return false;
        
    } catch (const std::exception& e) {
        std::cout << "❌ Login error: " << e.what() << std::endl;
        return false;
    }
}

bool DatabaseManager::updateUserProfile(const User& user) {
    if (!connected || user.profileId == -1) return false;
    
    try {
        auto conn = getConnection();
        
        std::vector<const char*> params = {
            user.subjects.c_str(),
            user.location.c_str(),
            user.name.c_str(),
            std::to_string(user.gradYear).c_str(),
            std::to_string(user.profileId).c_str()
        };
        
        PGresult* res = conn->execParams(
            "UPDATE profile SET subjects = $1, location = $2, name = $3, grad_year = $4 WHERE id = $5",
            params
        );
        PQclear(res);
        
        std::cout << "✅ Profile updated successfully!" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cout << "❌ Profile update failed: " << e.what() << std::endl;
        return false;
    }
}

std::vector<Group> DatabaseManager::getAllGroups() {
    std::vector<Group> groups;
    if (!connected) return groups;
    
    try {
        auto conn = getConnection();
        PGresult* res = conn->exec("SELECT id, name, description, subjects, location, member_count FROM study_groups");
        
        int rows = PQntuples(res);
        for (int i = 0; i < rows; i++) {
            Group group;
            group.id = std::stoi(PQgetvalue(res, i, 0));
            group.name = PQgetvalue(res, i, 1);
            group.description = PQgetvalue(res, i, 2) ? PQgetvalue(res, i, 2) : "";
            group.subjects = PQgetvalue(res, i, 3);
            group.location = PQgetvalue(res, i, 4) ? PQgetvalue(res, i, 4) : "";
            group.memberCount = std::stoi(PQgetvalue(res, i, 5));
            groups.push_back(group);
        }
        
        PQclear(res);
        
    } catch (const std::exception& e) {
        std::cout << "❌ Error fetching groups: " << e.what() << std::endl;
    }
    
    return groups;
}

std::vector<Group> DatabaseManager::getMatchingGroups(const std::vector<std::string>& userSubjects) {
    std::vector<Group> allGroups = getAllGroups();
    std::vector<Group> matchingGroups;
    
    for (const auto& group : allGroups) {
        for (const auto& userSubject : userSubjects) {
            // Case-insensitive matching
            std::string groupSubjectsLower = group.subjects;
            std::string userSubjectLower = userSubject;
            
            std::transform(groupSubjectsLower.begin(), groupSubjectsLower.end(), 
                          groupSubjectsLower.begin(), ::tolower);
            std::transform(userSubjectLower.begin(), userSubjectLower.end(), 
                          userSubjectLower.begin(), ::tolower);
            
            if (groupSubjectsLower.find(userSubjectLower) != std::string::npos) {
                matchingGroups.push_back(group);
                break;
            }
        }
    }
    
    return matchingGroups;
}

bool DatabaseManager::createGroup(const Group& group) {
    if (!connected) return false;
    
    try {
        auto conn = getConnection();
        
        std::vector<const char*> params = {
            group.name.c_str(),
            group.description.c_str(),
            group.subjects.c_str(),
            group.location.c_str(),
            std::to_string(group.memberCount).c_str()
        };
        
        PGresult* res = conn->execParams(
            "INSERT INTO study_groups (name, description, subjects, location, member_count) "
            "VALUES ($1, $2, $3, $4, $5)",
            params
        );
        PQclear(res);
        
        std::cout << "✅ Group created successfully!" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cout << "❌ Group creation failed: " << e.what() << std::endl;
        return false;
    }
}