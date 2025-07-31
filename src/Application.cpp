#include "Application.h"
#include "pg_connector.hpp"

Application::Application() : window(sf::VideoMode(1000, 700), "Study Sync Pro"), 
                            currentPage(PageType::LOGIN), isLoggedIn(false), showMessageText(false) {
    std::cout << "Starting Study Sync Pro application..." << std::endl;
    
    if (!font.loadFromFile("./fonts/arial.ttf")) {
        std::cout << "❌ Error loading font! Make sure arial.ttf is in the fonts/ directory" << std::endl;
        std::cout << "Trying alternative font loading..." << std::endl;
        if (!font.loadFromFile("fonts/arial.ttf")) {
            std::cout << "❌ Could not load font from fonts/arial.ttf either" << std::endl;
            std::cout << "Application will continue but text may not display properly" << std::endl;
        } else {
            std::cout << "✅ Font loaded from fonts/arial.ttf" << std::endl;
        }
    } else {
        std::cout << "✅ Font loaded successfully!" << std::endl;
    }
    
    // Initialize database connection
    initializeDatabase();
    
    std::cout << "Setting up initial page..." << std::endl;
    setupCurrentPage();
    std::cout << "✅ Application ready!" << std::endl;
}

Application::~Application() {
    clearTextBoxes();
}

void Application::initializeDatabase() {
    std::cout << "Testing database connection..." << std::endl;
    
    try {
        // Test your exact connection string
        std::string connStr = "postgresql://postgres:cen3031group4@db.iekosjtwireodvbaqhcm.supabase.co:5432/postgres";
        
        PgConnector db(connStr);
        
        // Try to get groups from your actual database
        PGresult* res = db.exec("SELECT group_name, description FROM groups");
        int groupCount = PQntuples(res);
        
        std::cout << "✅ Database connected! Found " << groupCount << " groups in database" << std::endl;
        
        // Load actual groups from database
        for (int i = 0; i < groupCount; i++) {
            Group group;
            group.name = PQgetvalue(res, i, 0);
            group.description = PQgetvalue(res, i, 1) ? PQgetvalue(res, i, 1) : "";
            group.subjects = group.name; // Use group name for subject matching
            group.location = "Various Locations";
            group.memberCount = 5 + (i % 10); // Mock member count
            groups.push_back(group);
        }
        PQclear(res);
        
        if (groups.empty()) {
            std::cout << "No groups found in database, using sample data" << std::endl;
            // Fallback to sample data
            groups.push_back({"Algebra Buddies", "Group focused on Algebra help and practice", "algebra,math", "Study Hall A", 8});
            groups.push_back({"Bio Crash Course", "Study group for AP Biology review sessions", "biology,science", "Library Room 202", 12});
            groups.push_back({"CS101 Legends", "Intro to programming and data structures", "programming,computer science", "Computer Lab", 15});
        }
        
    } catch (const std::exception& e) {
        std::cout << "❌ Database connection failed: " << e.what() << std::endl;
        
        // Fallback to sample data
        groups.push_back({"Algebra Buddies", "Group focused on Algebra help and practice", "algebra,math", "Study Hall A", 8});
        groups.push_back({"Bio Crash Course", "Study group for AP Biology review sessions", "biology,science", "Library Room 202", 12});
        groups.push_back({"CS101 Legends", "Intro to programming and data structures", "programming,computer science", "Computer Lab", 15});
        groups.push_back({"English Essay Editors", "Peer editing group for literature assignments", "english,literature", "English Department", 6});
        groups.push_back({"SAT Prep Circle", "Preparing for SAT with weekly practice tests", "math,english,test prep", "Testing Center", 10});
    }
    
    std::cout << "Loaded " << groups.size() << " study groups" << std::endl;
}

void Application::run() {
    std::cout << "Starting main application loop..." << std::endl;
    
    while (window.isOpen()) {
        handleEvents();
        update();
        render();
    }
    
    std::cout << "Application loop ended." << std::endl;
}

std::vector<std::string> Application::parseSubjects(const std::string& subjects) {
    std::vector<std::string> result;
    std::string delimiter = ",";
    size_t pos = 0;
    std::string token;
    std::string subjectsCopy = subjects;
    
    while ((pos = subjectsCopy.find(delimiter)) != std::string::npos) {
        token = subjectsCopy.substr(0, pos);
        // Remove spaces
        token.erase(std::remove_if(token.begin(), token.end(), ::isspace), token.end());
        if (!token.empty()) {
            result.push_back(token);
        }
        subjectsCopy.erase(0, pos + delimiter.length());
    }
    if (!subjectsCopy.empty()) {
        subjectsCopy.erase(std::remove_if(subjectsCopy.begin(), subjectsCopy.end(), ::isspace), subjectsCopy.end());
        result.push_back(subjectsCopy);
    }
    
    return result;
}

void Application::clearTextBoxes() {
    for (auto* textBox : textBoxes) {
        delete textBox;
    }
    textBoxes.clear();
    buttons.clear();
    buttonTexts.clear();
    labels.clear();
}

void Application::setupCurrentPage() {
    clearTextBoxes();
    showMessageText = false;
    
    switch (currentPage) {
        case PageType::LOGIN:
            setupLoginPage();
            break;
        case PageType::REGISTRATION:
            setupRegistrationPage();
            break;
        case PageType::PROFILE:
            setupProfilePage();
            break;
        case PageType::GROUP_MATCHING:
            setupGroupMatchingPage();
            break;
    }
}

void Application::setupLoginPage() {
    titleText.setFont(font);
    titleText.setString("Study Sync Pro");
    titleText.setCharacterSize(42);
    titleText.setFillColor(sf::Color(64, 156, 255)); // Modern blue
    titleText.setStyle(sf::Text::Bold);
    centerText(titleText, 120);
    
    // Subtitle
    sf::Text subtitleText;
    subtitleText.setFont(font);
    subtitleText.setString("Connect with your academic community");
    subtitleText.setCharacterSize(18);
    subtitleText.setFillColor(sf::Color(180, 180, 180));
    centerText(subtitleText, 170);
    
    textBoxes.push_back(new TextBox(350, 240, 300, 45, "Username:", font));
    textBoxes.push_back(new TextBox(350, 320, 300, 45, "Password:", font));
    
    createButton(400, 400, 200, 50, "Login", sf::Color(64, 156, 255));
    createButton(400, 470, 200, 50, "Create Account", sf::Color(34, 197, 94));
    
    showMessageText = true;
    messageText.setFont(font);
    messageText.setCharacterSize(16);
    messageText.setFillColor(sf::Color(239, 68, 68));
    messageText.setPosition(350, 550);
    messageText.setString("");
}

void Application::setupRegistrationPage() {
    titleText.setFont(font);
    titleText.setString("Create Your Account");
    titleText.setCharacterSize(36);
    titleText.setFillColor(sf::Color(64, 156, 255));
    titleText.setStyle(sf::Text::Bold);
    centerText(titleText, 60);
    
    // Subtitle
    sf::Text subtitleText;
    subtitleText.setFont(font);
    subtitleText.setString("Join the study community");
    subtitleText.setCharacterSize(16);
    subtitleText.setFillColor(sf::Color(160, 160, 160));
    centerText(subtitleText, 100);
    
    // Better spacing and layout
    textBoxes.push_back(new TextBox(350, 140, 300, 45, "Full Name:", font));
    textBoxes.push_back(new TextBox(350, 200, 300, 45, "Graduation Year:", font));
    textBoxes.push_back(new TextBox(350, 260, 300, 45, "Current Classes:", font));
    textBoxes.push_back(new TextBox(350, 320, 300, 45, "Username:", font));
    textBoxes.push_back(new TextBox(350, 380, 300, 45, "Password:", font));
    textBoxes.push_back(new TextBox(350, 440, 300, 45, "Confirm Password:", font));
    
    createButton(375, 520, 120, 50, "Register", sf::Color(34, 197, 94));
    createButton(505, 520, 120, 50, "Back", sf::Color(107, 114, 128));
    
    showMessageText = true;
    messageText.setFont(font);
    messageText.setCharacterSize(16);
    messageText.setFillColor(sf::Color(239, 68, 68));
    messageText.setPosition(350, 590);
    messageText.setString("");
}

void Application::setupProfilePage() {
    titleText.setFont(font);
    titleText.setString("Welcome, " + currentUser.name + "!");
    titleText.setCharacterSize(32);
    titleText.setFillColor(sf::Color(64, 156, 255));
    titleText.setStyle(sf::Text::Bold);
    centerText(titleText, 100);
    
    // User info display
    sf::Text infoText;
    infoText.setFont(font);
    infoText.setString("Class of " + std::to_string(currentUser.gradYear));
    infoText.setCharacterSize(18);
    infoText.setFillColor(sf::Color(160, 160, 160));
    centerText(infoText, 140);
    
    // Match database field names with better spacing
    textBoxes.push_back(new TextBox(350, 200, 300, 45, "Current Classes:", font));
    textBoxes.push_back(new TextBox(350, 270, 300, 45, "Graduation Year:", font));
    
    if (!currentUser.subjects.empty()) {
        textBoxes[0]->setContent(currentUser.subjects);
    }
    textBoxes[1]->setContent(std::to_string(currentUser.gradYear));
    
    createButton(300, 360, 140, 50, "Save Profile", sf::Color(34, 197, 94));
    createButton(450, 360, 140, 50, "Find Groups", sf::Color(168, 85, 247));
    createButton(450, 430, 140, 50, "Logout", sf::Color(239, 68, 68));
    
    showMessageText = true;
    messageText.setFont(font);
    messageText.setCharacterSize(16);
    messageText.setFillColor(sf::Color(34, 197, 94));
    messageText.setPosition(350, 500);
    messageText.setString("");
}

void Application::setupGroupMatchingPage() {
    titleText.setFont(font);
    titleText.setString("Your Study Groups");
    titleText.setCharacterSize(36);
    titleText.setFillColor(sf::Color(64, 156, 255));
    titleText.setStyle(sf::Text::Bold);
    centerText(titleText, 60);
    
    // Subtitle
    sf::Text subtitleText;
    subtitleText.setFont(font);
    subtitleText.setString("Groups matching your classes");
    subtitleText.setCharacterSize(16);
    subtitleText.setFillColor(sf::Color(160, 160, 160));
    centerText(subtitleText, 100);
    
    createButton(450, 600, 100, 50, "Back", sf::Color(107, 114, 128));
    
    showMessageText = false;
}

void Application::createButton(float x, float y, float width, float height, const std::string& text, sf::Color color) {
    sf::RectangleShape button(sf::Vector2f(width, height));
    button.setPosition(x, y);
    button.setFillColor(color);
    button.setOutlineThickness(0); // Remove outline for modern look
    buttons.push_back(button);
    
    sf::Text buttonText;
    buttonText.setFont(font);
    buttonText.setString(text);
    buttonText.setCharacterSize(16);
    buttonText.setFillColor(sf::Color::White);
    buttonText.setStyle(sf::Text::Bold);
    
    sf::FloatRect textBounds = buttonText.getLocalBounds();
    buttonText.setPosition(
        x + (width - textBounds.width) / 2.0f,
        y + (height - textBounds.height) / 2.0f - 3
    );
    buttonTexts.push_back(buttonText);
}

void Application::centerText(sf::Text& text, float y) {
    sf::FloatRect bounds = text.getLocalBounds();
    text.setPosition((1000 - bounds.width) / 2.0f, y); // Updated for new window width
}

void Application::handleEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();
        
        for (auto* textBox : textBoxes) {
            textBox->handleEvent(event);
        }
        
        if (event.type == sf::Event::MouseButtonPressed) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            handleButtonClicks(mousePos);
        }
    }
}

void Application::handleButtonClicks(sf::Vector2i mousePos) {
    for (size_t i = 0; i < buttons.size(); ++i) {
        sf::FloatRect buttonBounds = buttons[i].getGlobalBounds();
        if (buttonBounds.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
            handleButtonClick(i);
            break;
        }
    }
}

void Application::handleButtonClick(size_t buttonIndex) {
    switch (currentPage) {
        case PageType::LOGIN:
            handleLoginButtons(buttonIndex);
            break;
        case PageType::REGISTRATION:
            handleRegistrationButtons(buttonIndex);
            break;
        case PageType::PROFILE:
            handleProfileButtons(buttonIndex);
            break;
        case PageType::GROUP_MATCHING:
            handleGroupMatchingButtons(buttonIndex);
            break;
    }
}

void Application::handleLoginButtons(size_t buttonIndex) {
    if (buttonIndex == 0) { // Login
        std::string username = textBoxes[0]->getContent();
        std::string password = textBoxes[1]->getContent();
        
        // Try database login
        try {
            std::string connStr = "postgresql://postgres:cen3031group4@db.iekosjtwireodvbaqhcm.supabase.co:5432/postgres";
            PgConnector db(connStr);
            
            // Check username and password directly (plain text)
            std::vector<const char*> params = {username.c_str(), password.c_str()};
            PGresult* res = db.execParams(
                "SELECT u.username, p.name, p.grad_year, p.current_classes, p.id "
                "FROM user_login u JOIN profile p ON u.profile_id = p.id "
                "WHERE u.username = $1 AND u.password_hash = $2", 
                params
            );
            
            if (PQntuples(res) > 0) {
                currentUser.username = PQgetvalue(res, 0, 0);
                currentUser.name = PQgetvalue(res, 0, 1);
                currentUser.gradYear = std::stoi(PQgetvalue(res, 0, 2));
                currentUser.subjects = PQgetvalue(res, 0, 3) ? PQgetvalue(res, 0, 3) : "";
                currentUser.profileId = std::stoi(PQgetvalue(res, 0, 4));
                
                PQclear(res);
                
                isLoggedIn = true;
                currentPage = PageType::PROFILE;
                setupCurrentPage();
                std::cout << "✅ Database login successful for: " << username << std::endl;
                return;
            }
            PQclear(res);
            
        } catch (const std::exception& e) {
            std::cout << "Database login failed: " << e.what() << std::endl;
        }
        
        // Fallback to simple test login
        if (username == "test" && password == "test") {
            currentUser.username = username;
            currentUser.name = "Test User";
            currentUser.subjects = "Math, Science";
            isLoggedIn = true;
            currentPage = PageType::PROFILE;
            setupCurrentPage();
        } else {
            messageText.setString("Invalid username or password!");
        }
    } else if (buttonIndex == 1) { // Register
        currentPage = PageType::REGISTRATION;
        setupCurrentPage();
    }
}

void Application::handleRegistrationButtons(size_t buttonIndex) {
    if (buttonIndex == 0) { // Register
        std::string name = textBoxes[0]->getContent();
        std::string gradYearStr = textBoxes[1]->getContent();
        std::string currentClasses = textBoxes[2]->getContent(); // Changed from subjects
        std::string username = textBoxes[3]->getContent();
        std::string password = textBoxes[4]->getContent();
        std::string confirmPass = textBoxes[5]->getContent();
        
        if (name.empty() || gradYearStr.empty() || currentClasses.empty() || username.empty() || password.empty()) {
            messageText.setString("Please fill all fields!");
            return;
        }
        
        if (password != confirmPass) {
            messageText.setString("Passwords don't match!");
            return;
        }
        
        int gradYear = 0;
        try {
            gradYear = std::stoi(gradYearStr);
        } catch (...) {
            messageText.setString("Invalid graduation year!");
            return;
        }
        
        // Try to register in database
        try {
            std::string connStr = "postgresql://postgres:cen3031group4@db.iekosjtwireodvbaqhcm.supabase.co:5432/postgres";
            PgConnector db(connStr);
            
            // Begin transaction
            PGresult* beginRes = db.exec("BEGIN");
            PQclear(beginRes);
            
            // Check if username already exists
            std::vector<const char*> checkParams = {username.c_str()};
            PGresult* checkRes = db.execParams("SELECT username FROM user_login WHERE username = $1", checkParams);
            
            if (PQntuples(checkRes) > 0) {
                PQclear(checkRes);
                PGresult* rollbackRes = db.exec("ROLLBACK");
                PQclear(rollbackRes);
                messageText.setString("Username already exists!");
                return;
            }
            PQclear(checkRes);
            
            // Insert into profile table with correct field names
            std::vector<const char*> profileParams = {name.c_str(), std::to_string(gradYear).c_str(), currentClasses.c_str()};
            PGresult* profileRes = db.execParams(
                "INSERT INTO profile (name, grad_year, current_classes) VALUES ($1, $2, $3) RETURNING id",
                profileParams
            );
            
            if (PQntuples(profileRes) == 0) {
                PQclear(profileRes);
                PGresult* rollbackRes = db.exec("ROLLBACK");
                PQclear(rollbackRes);
                messageText.setString("Failed to create profile!");
                return;
            }
            
            std::string profileId = PQgetvalue(profileRes, 0, 0);
            PQclear(profileRes);
            
            // Insert into user_login with plain text password
            std::vector<const char*> loginParams = {username.c_str(), password.c_str(), profileId.c_str()};
            PGresult* loginRes = db.execParams(
                "INSERT INTO user_login (username, password_hash, profile_id) VALUES ($1, $2, $3)",
                loginParams
            );
            PQclear(loginRes);
            
            // Commit transaction
            PGresult* commitRes = db.exec("COMMIT");
            PQclear(commitRes);
            
            // Set current user and login
            currentUser.name = name;
            currentUser.username = username;
            currentUser.subjects = currentClasses; // Store in subjects field for compatibility
            currentUser.gradYear = gradYear;
            currentUser.profileId = std::stoi(profileId);
            
            isLoggedIn = true;
            currentPage = PageType::PROFILE;
            setupCurrentPage();
            std::cout << "✅ User registered successfully in database: " << username << std::endl;
            return;
            
        } catch (const std::exception& e) {
            std::cout << "Database registration failed: " << e.what() << std::endl;
            messageText.setString("Registration failed! Try again.");
            return;
        }
        
    } else if (buttonIndex == 1) { // Back
        currentPage = PageType::LOGIN;
        setupCurrentPage();
    }
}

void Application::handleProfileButtons(size_t buttonIndex) {
    if (buttonIndex == 0) { // Save Profile
        std::string currentClasses = textBoxes[0]->getContent();
        std::string gradYearStr = textBoxes[1]->getContent();
        
        int gradYear = 0;
        try {
            gradYear = std::stoi(gradYearStr);
        } catch (...) {
            messageText.setString("Invalid graduation year!");
            return;
        }
        
        // Try to update in database
        try {
            std::string connStr = "postgresql://postgres:cen3031group4@db.iekosjtwireodvbaqhcm.supabase.co:5432/postgres";
            PgConnector db(connStr);
            
            std::vector<const char*> params = {
                currentClasses.c_str(),
                std::to_string(gradYear).c_str(),
                std::to_string(currentUser.profileId).c_str()
            };
            
            PGresult* res = db.execParams(
                "UPDATE profile SET current_classes = $1, grad_year = $2 WHERE id = $3",
                params
            );
            PQclear(res);
            
            // Update local user data
            currentUser.subjects = currentClasses;
            currentUser.gradYear = gradYear;
            
            messageText.setString("Profile saved successfully!");
            std::cout << "✅ Profile updated in database" << std::endl;
            
        } catch (const std::exception& e) {
            std::cout << "Database update failed: " << e.what() << std::endl;
            messageText.setString("Failed to save profile!");
        }
        
    } else if (buttonIndex == 1) { // Find Groups
        if (currentUser.subjects.empty()) {
            messageText.setString("Please add current classes first!");
        } else {
            currentPage = PageType::GROUP_MATCHING;
            setupCurrentPage();
        }
    } else if (buttonIndex == 2) { // Logout
        isLoggedIn = false;
        currentUser = User{};
        currentPage = PageType::LOGIN;
        setupCurrentPage();
    }
}

void Application::handleGroupMatchingButtons(size_t buttonIndex) {
    if (buttonIndex == 0) { // Back
        currentPage = PageType::PROFILE;
        setupCurrentPage();
    }
}

std::vector<Group> Application::getMatchedGroups() {
    std::vector<Group> matches;
    std::vector<std::string> userSubjects = parseSubjects(currentUser.subjects);
    
    for (const auto& group : groups) {
        for (const auto& userSubject : userSubjects) {
            if (group.subjects.find(userSubject) != std::string::npos) {
                matches.push_back(group);
                break;
            }
        }
    }
    return matches;
}

void Application::update() {
    for (auto* textBox : textBoxes) {
        textBox->update();
    }
}

void Application::render() {
    // Modern gradient background
    window.clear(sf::Color(15, 23, 42)); // Dark blue-gray background
    
    window.draw(titleText);
    
    for (auto* textBox : textBoxes) {
        textBox->draw(window);
    }
    
    for (size_t i = 0; i < buttons.size(); ++i) {
        window.draw(buttons[i]);
        window.draw(buttonTexts[i]);
    }
    
    if (showMessageText) {
        window.draw(messageText);
    }
    
    if (currentPage == PageType::GROUP_MATCHING) {
        renderGroupMatches();
    }
    
    window.display();
}

void Application::renderGroupMatches() {
    auto matches = getMatchedGroups();
    float yPos = 140;
    
    for (size_t i = 0; i < matches.size() && i < 6; ++i) {
        const auto& group = matches[i];
        
        // Create a card-like background for each group
        sf::RectangleShape groupCard(sf::Vector2f(800, 70));
        groupCard.setPosition(100, yPos);
        groupCard.setFillColor(sf::Color(30, 41, 59)); // Darker card background
        groupCard.setOutlineThickness(1);
        groupCard.setOutlineColor(sf::Color(51, 65, 85));
        window.draw(groupCard);
        
        // Group name
        sf::Text groupName;
        groupName.setFont(font);
        groupName.setString(group.name);
        groupName.setCharacterSize(20);
        groupName.setFillColor(sf::Color(245, 245, 245));
        groupName.setStyle(sf::Text::Bold);
        groupName.setPosition(120, yPos + 8);
        window.draw(groupName);
        
        // Member count badge
        sf::CircleShape memberBadge(12);
        memberBadge.setPosition(820, yPos + 10);
        memberBadge.setFillColor(sf::Color(34, 197, 94));
        window.draw(memberBadge);
        
        sf::Text memberCount;
        memberCount.setFont(font);
        memberCount.setString(std::to_string(group.memberCount));
        memberCount.setCharacterSize(12);
        memberCount.setFillColor(sf::Color::White);
        memberCount.setStyle(sf::Text::Bold);
        memberCount.setPosition(826, yPos + 14);
        window.draw(memberCount);
        
        // Group description
        sf::Text groupDesc;
        groupDesc.setFont(font);
        groupDesc.setString(group.description);
        groupDesc.setCharacterSize(14);
        groupDesc.setFillColor(sf::Color(156, 163, 175));
        groupDesc.setPosition(120, yPos + 32);
        window.draw(groupDesc);
        
        // Location
        sf::Text locationText;
        locationText.setFont(font);
        locationText.setString("📍 " + group.location);
        locationText.setCharacterSize(12);
        locationText.setFillColor(sf::Color(107, 114, 128));
        locationText.setPosition(120, yPos + 52);
        window.draw(locationText);
        
        yPos += 80;
    }
    
    if (matches.empty()) {
        sf::Text noMatches;
        noMatches.setFont(font);
        noMatches.setString("No matching study groups found yet.\nTry updating your classes in your profile!");
        noMatches.setCharacterSize(18);
        noMatches.setFillColor(sf::Color(156, 163, 175));
        centerText(noMatches, 300);
        window.draw(noMatches);
    }
}
