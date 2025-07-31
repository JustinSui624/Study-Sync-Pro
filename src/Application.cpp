#include "Application.h"

Application::Application() : window(sf::VideoMode(800, 600), "Study Group Finder"), 
                            currentPage(PageType::LOGIN), isLoggedIn(false), showMessageText(false) {
    std::cout << "Starting Study Group Finder application..." << std::endl;
    
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
    std::cout << "Attempting to connect to database..." << std::endl;
    
    // Your Supabase connection string
    std::string connectionString = "postgresql://postgres:cen3031group4@db.iekosjtwireodvbaqhcm.supabase.co:5432/postgres";
    
    try {
        std::cout << "Creating DatabaseManager..." << std::endl;
        dbManager = std::make_unique<DatabaseManager>(connectionString);
        std::cout << "DatabaseManager created." << std::endl;
        
        if (dbManager->isConnected()) {
            std::cout << "Database connected successfully!" << std::endl;
            dbManager->initializeTables();
            
            // Load groups from database
            groups = dbManager->getAllGroups();
            std::cout << "✅ Loaded " << groups.size() << " study groups from database" << std::endl;
        } else {
            std::cout << "❌ Database connection failed. Running in offline mode." << std::endl;
            // Add some sample groups for offline mode
            groups.push_back({"Sample Math Group", "Basic math tutoring", "math,algebra", "Local University", 5});
            groups.push_back({"Science Study Circle", "Biology and chemistry", "biology,chemistry,science", "Community College", 8});
        }
        
    } catch (const std::exception& e) {
        std::cout << "❌ Database initialization failed: " << e.what() << std::endl;
        std::cout << "Running in offline mode with sample data." << std::endl;
        dbManager = nullptr;
        
        // Add sample groups for offline mode
        groups.push_back({"Sample Math Group", "Basic math tutoring", "math,algebra", "Local University", 5});
        groups.push_back({"Science Study Circle", "Biology and chemistry", "biology,chemistry,science", "Community College", 8});
    }
    
    std::cout << "Database initialization complete." << std::endl;
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
    titleText.setString("Login");
    titleText.setCharacterSize(32);
    titleText.setFillColor(sf::Color::White);
    titleText.setStyle(sf::Text::Bold);
    centerText(titleText, 100);
    
    textBoxes.push_back(new TextBox(250, 180, 300, 35, "Username:", font));
    textBoxes.push_back(new TextBox(250, 250, 300, 35, "Password:", font));
    
    createButton(325, 320, 150, 40, "Login", sf::Color::Blue);
    createButton(325, 380, 150, 40, "Register", sf::Color::Green);
    
    showMessageText = true;
    messageText.setFont(font);
    messageText.setCharacterSize(16);
    messageText.setFillColor(sf::Color::Red);
    messageText.setPosition(250, 450);
    messageText.setString("");
}

void Application::setupRegistrationPage() {
    titleText.setFont(font);
    titleText.setString("Registration");
    titleText.setCharacterSize(32);
    titleText.setFillColor(sf::Color::White);
    titleText.setStyle(sf::Text::Bold);
    centerText(titleText, 30);
    
    textBoxes.push_back(new TextBox(250, 120, 300, 35, "Full Name:", font));
    textBoxes.push_back(new TextBox(250, 185, 300, 35, "Graduation Year:", font));
    textBoxes.push_back(new TextBox(250, 250, 300, 35, "Subjects (comma separated):", font));
    textBoxes.push_back(new TextBox(250, 315, 300, 35, "Username:", font));
    textBoxes.push_back(new TextBox(250, 380, 300, 35, "Password:", font));
    textBoxes.push_back(new TextBox(250, 445, 300, 35, "Confirm Password:", font));
    
    createButton(275, 510, 100, 40, "Register", sf::Color::Blue);
    createButton(425, 510, 100, 40, "Back", sf::Color(128, 128, 128));
    
    showMessageText = true;
    messageText.setFont(font);
    messageText.setCharacterSize(16);
    messageText.setFillColor(sf::Color::Red);
    messageText.setPosition(250, 570);
    messageText.setString("");
}

void Application::setupProfilePage() {
    titleText.setFont(font);
    titleText.setString("Profile - " + currentUser.name);
    titleText.setCharacterSize(28);
    titleText.setFillColor(sf::Color::White);
    titleText.setStyle(sf::Text::Bold);
    centerText(titleText, 80);
    
    textBoxes.push_back(new TextBox(250, 150, 300, 35, "Subjects (comma separated):", font));
    textBoxes.push_back(new TextBox(250, 220, 300, 35, "School/University:", font));
    
    if (!currentUser.subjects.empty()) {
        textBoxes[0]->setContent(currentUser.subjects);
    }
    if (!currentUser.location.empty()) {
        textBoxes[1]->setContent(currentUser.location);
    }
    
    createButton(250, 290, 120, 40, "Save Profile", sf::Color::Blue);
    createButton(380, 290, 120, 40, "Find Groups", sf::Color::Green);
    createButton(510, 290, 90, 40, "Logout", sf::Color::Red);
    
    showMessageText = true;
    messageText.setFont(font);
    messageText.setCharacterSize(16);
    messageText.setFillColor(sf::Color::Green);
    messageText.setPosition(250, 360);
    messageText.setString("");
}

void Application::setupGroupMatchingPage() {
    titleText.setFont(font);
    titleText.setString("Recommended Study Groups");
    titleText.setCharacterSize(28);
    titleText.setFillColor(sf::Color::White);
    titleText.setStyle(sf::Text::Bold);
    centerText(titleText, 40);
    
    createButton(350, 520, 100, 40, "Back", sf::Color(128, 128, 128));
    
    showMessageText = false;
}

void Application::createButton(float x, float y, float width, float height, const std::string& text, sf::Color color) {
    sf::RectangleShape button(sf::Vector2f(width, height));
    button.setPosition(x, y);
    button.setFillColor(color);
    button.setOutlineThickness(2);
    button.setOutlineColor(sf::Color::White);
    buttons.push_back(button);
    
    sf::Text buttonText;
    buttonText.setFont(font);
    buttonText.setString(text);
    buttonText.setCharacterSize(16);
    buttonText.setFillColor(sf::Color::White);
    
    sf::FloatRect textBounds = buttonText.getLocalBounds();
    buttonText.setPosition(
        x + (width - textBounds.width) / 2.0f,
        y + (height - textBounds.height) / 2.0f - 5
    );
    buttonTexts.push_back(buttonText);
}

void Application::centerText(sf::Text& text, float y) {
    sf::FloatRect bounds = text.getLocalBounds();
    text.setPosition((800 - bounds.width) / 2.0f, y);
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
        
        if (dbManager && dbManager->authenticateUser(username, password, currentUser)) {
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
        std::string subjects = textBoxes[2]->getContent();
        std::string username = textBoxes[3]->getContent();
        std::string password = textBoxes[4]->getContent();
        std::string confirmPass = textBoxes[5]->getContent();
        
        if (name.empty() || gradYearStr.empty() || subjects.empty() || username.empty() || password.empty()) {
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
        
        if (dbManager && dbManager->createProfileAndLogin(name, gradYear, subjects, username, password)) {
            // After successful registration, log them in
            if (dbManager->authenticateUser(username, password, currentUser)) {
                isLoggedIn = true;
                currentPage = PageType::PROFILE;
                setupCurrentPage();
            }
        } else {
            messageText.setString("Registration failed! Username might already exist.");
        }
        
    } else if (buttonIndex == 1) { // Back
        currentPage = PageType::LOGIN;
        setupCurrentPage();
    }
}

void Application::handleProfileButtons(size_t buttonIndex) {
    if (buttonIndex == 0) { // Save Profile
        currentUser.subjects = textBoxes[0]->getContent();
        currentUser.location = textBoxes[1]->getContent();
        
        if (dbManager && dbManager->updateUserProfile(currentUser)) {
            messageText.setString("Profile saved successfully!");
        } else {
            messageText.setString("Failed to save profile!");
        }
        
    } else if (buttonIndex == 1) { // Find Groups
        if (currentUser.subjects.empty()) {
            messageText.setString("Please add subjects first!");
        } else {
            // Refresh groups from database before matching
            if (dbManager) {
                groups = dbManager->getAllGroups();
            }
            currentPage = PageType::GROUP_MATCHING;
            setupCurrentPage();
        }
    } else if (buttonIndex == 2) { // Logout
        isLoggedIn = false;
        currentUser = User{}; // Reset current user
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
    if (dbManager) {
        std::vector<std::string> userSubjects = parseSubjects(currentUser.subjects);
        return dbManager->getMatchingGroups(userSubjects);
    } else {
        // Fallback to local matching if database is unavailable
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
}

void Application::update() {
    for (auto* textBox : textBoxes) {
        textBox->update();
    }
}

void Application::render() {
    window.clear(sf::Color::Black);
    
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
    float yPos = 90;
    
    for (size_t i = 0; i < matches.size() && i < 5; ++i) {
        const auto& group = matches[i];
        
        sf::Text groupName;
        groupName.setFont(font);
        groupName.setString(group.name + " (" + std::to_string(group.memberCount) + " members)");
        groupName.setCharacterSize(18);
        groupName.setFillColor(sf::Color::Yellow);
        groupName.setPosition(50, yPos);
        window.draw(groupName);
        
        sf::Text groupDesc;
        groupDesc.setFont(font);
        groupDesc.setString(group.description);
        groupDesc.setCharacterSize(14);
        groupDesc.setFillColor(sf::Color::White);
        groupDesc.setPosition(70, yPos + 25);
        window.draw(groupDesc);
        
        sf::Text groupInfo;
        groupInfo.setFont(font);
        groupInfo.setString("School: " + group.location + " | Subjects: " + group.subjects);
        groupInfo.setCharacterSize(12);
        groupInfo.setFillColor(sf::Color::Cyan);
        groupInfo.setPosition(70, yPos + 45);
        window.draw(groupInfo);
        
        yPos += 85;
    }
    
    if (matches.empty()) {
        sf::Text noMatches;
        noMatches.setFont(font);
        noMatches.setString("No matching study groups found. Try updating your subjects!");
        noMatches.setCharacterSize(16);
        noMatches.setFillColor(sf::Color::Red);
        noMatches.setPosition(150, 200);
        window.draw(noMatches);
    }
}