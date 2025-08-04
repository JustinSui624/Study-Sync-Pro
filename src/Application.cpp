#include "Application.h"
#include "pg_connector.hpp"


Application::Application() : window(sf::VideoMode(1000, 700), "Study Sync Pro"), 
                            currentPage(PageType::LOGIN), isLoggedIn(false), showMessageText(false), selectedGroupName("") {
    std::cout << "Starting Study Sync Pro application..." << std::endl;
    
    if (!font.loadFromFile("./fonts/arial.ttf")) {
        std::cout << "Error loading font! Make sure arial.ttf is in the fonts/ directory" << std::endl;
        std::cout << "Trying alternative font loading..." << std::endl;
        if (!font.loadFromFile("fonts/arial.ttf")) {
            std::cout << "Could not load font from fonts/arial.ttf either" << std::endl;
            std::cout << "Application will continue but text may not display properly" << std::endl;
        } else {
            std::cout << "Font loaded from fonts/arial.ttf" << std::endl;
        }
    } else {
        std::cout << "Font loaded successfully!" << std::endl;
    }
    
    // Initialize database connection
    initializeDatabase();
    
    std::cout << "Setting up initial page..." << std::endl;
    setupCurrentPage();
    std::cout << "Application ready!" << std::endl;
}

Application::~Application() {
    clearTextBoxes();
}

void Application::initializeDatabase() {
    std::cout << "Testing database connection..." << std::endl;
    
    try {
        std::string connStr = "postgresql://postgres:cen3031group4@db.iekosjtwireodvbaqhcm.supabase.co:5432/postgres";
        
        PgConnector db(connStr);
        
        // Get groups from database
        PGresult* res = db.exec("SELECT group_name, description, subjects, contact FROM groups");
        int groupCount = PQntuples(res);
        
        std::cout << "Database connected! Found " << groupCount << " groups in database" << std::endl;
        
        // Load actual groups from database
        for (int i = 0; i < groupCount; i++) {
            Group group;
            group.name = PQgetvalue(res, i, 0);           // group_name (column 0)
            group.description = PQgetvalue(res, i, 1) ? PQgetvalue(res, i, 1) : "";  // description (column 1)
            group.subjects = PQgetvalue(res, i, 2) ? PQgetvalue(res, i, 2) : "";     // subjects (column 2) 
            group.contactCell = PQgetvalue(res, i, 3) ? PQgetvalue(res, i, 3) : "No contact available";  // contact (column 3)
            group.location = "Various Locations";  // You could add this to your database table if needed
            groups.push_back(group);
        }
        PQclear(res);
        
        std::cout << "Loaded " << groups.size() << " study groups from database" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "Database connection failed: " << e.what() << std::endl;
        std::cout << "No groups loaded - application will show empty group list" << std::endl;
    }
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
    
    if (subjects.empty()) {
        return result;
    }
    
    std::stringstream ss(subjects);
    std::string item;
    
    while (std::getline(ss, item, ',')) {
        // Trim whitespace
        item.erase(0, item.find_first_not_of(" \t\r\n"));
        item.erase(item.find_last_not_of(" \t\r\n") + 1);
        
        if (!item.empty()) {
            result.push_back(item);
        }
    }
    
    return result;
}

void Application::clearTextBoxes() {
    for (auto* textBox : textBoxes) {
        delete textBox;
    }
    textBoxes.clear();
    
    for (auto* dropdown : dropdownMenus) {
        delete dropdown;
    }
    dropdownMenus.clear();
    
    for (auto* multiDropdown : multiSelectDropdowns) {
        delete multiDropdown;
    }
    multiSelectDropdowns.clear();
    
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
        case PageType::CONTACT:
            setupContactPage();
			break;
		case PageType::CREATE_GROUP:
            setupCreateGroupPage();
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
    centerText(titleText, 40);
    
    // Common subjects list
    std::vector<std::string> subjects = {
        "Mathematics", "Algebra", "Calculus", "Geometry", "Statistics",
        "Science", "Biology", "Chemistry", "Physics", "Environmental Science",
        "English", "Literature", "Creative Writing", "Grammar", "Public Speaking",
        "History", "World History", "US History", "European History", "Government",
        "Computer Science", "Programming", "Data Structures", "Algorithms", "Web Development",
        "Art", "Music", "Theater", "Photography", "Graphic Design",
        "Foreign Languages", "Spanish", "French", "German", "Chinese",
        "Economics", "Psychology", "Sociology", "Philosophy", "Business"
    };
    
    const float startY = 120;
    const float spacing = 80;
    
    textBoxes.push_back(new TextBox(350, startY, 300, 45, "Full Name:", font));
    textBoxes.push_back(new TextBox(350, startY + spacing, 300, 45, "Graduation Year:", font));
    
    // Multi-select subject dropdown with proper label
    sf::Text subjectLabel;
    subjectLabel.setFont(font);
    subjectLabel.setString("Subjects (select multiple):");
    subjectLabel.setCharacterSize(16);
    subjectLabel.setFillColor(sf::Color::White);
    subjectLabel.setPosition(350, startY + 2 * spacing - 25);
    labels.push_back(subjectLabel);
    
    multiSelectDropdowns.push_back(new MultiSelectDropdown(350, startY + 2 * spacing, 300, 45, font, subjects, 5));
    
    textBoxes.push_back(new TextBox(350, startY + 3 * spacing, 300, 45, "Username:", font));
    textBoxes.push_back(new TextBox(350, startY + 4 * spacing, 300, 45, "Password:", font));
    textBoxes.push_back(new TextBox(350, startY + 5 * spacing, 300, 45, "Confirm Password:", font));
    
    createButton(350, startY + 6 * spacing + 30, 140, 50, "Register", sf::Color(34, 197, 94));
    createButton(510, startY + 6 * spacing + 30, 140, 50, "Back", sf::Color(107, 114, 128));
    
    showMessageText = true;
    messageText.setFont(font);
    messageText.setCharacterSize(16);
    messageText.setFillColor(sf::Color(239, 68, 68));
    messageText.setPosition(350, startY + 7 * spacing + 30);
    messageText.setString("");
}

void Application::setupProfilePage() {
    titleText.setFont(font);
    titleText.setString("Welcome, " + currentUser.name + "!");
    titleText.setCharacterSize(32);
    titleText.setFillColor(sf::Color(64, 156, 255));
    titleText.setStyle(sf::Text::Bold);
    centerText(titleText, 100);
    
    // Common subjects list
    std::vector<std::string> subjects = {
        "Mathematics", "Algebra", "Calculus", "Geometry", "Statistics",
        "Science", "Biology", "Chemistry", "Physics", "Environmental Science",
        "English", "Literature", "Creative Writing", "Grammar", "Public Speaking",
        "History", "World History", "US History", "European History", "Government",
        "Computer Science", "Programming", "Data Structures", "Algorithms", "Web Development",
        "Art", "Music", "Theater", "Photography", "Graphic Design",
        "Foreign Languages", "Spanish", "French", "German", "Chinese",
        "Economics", "Psychology", "Sociology", "Philosophy", "Business"
    };
    
    // Subject multi-select dropdown with proper label
    sf::Text subjectLabel;
    subjectLabel.setFont(font);
    subjectLabel.setString("Subjects (select multiple):");
    subjectLabel.setCharacterSize(16);
    subjectLabel.setFillColor(sf::Color::White);
    subjectLabel.setPosition(350, 175);
    labels.push_back(subjectLabel);
    
    multiSelectDropdowns.push_back(new MultiSelectDropdown(350, 200, 300, 45, font, subjects, 5));
    
    // Set existing subjects if any
    if (!currentUser.subjects.empty()) {
        auto userSubjects = parseSubjects(currentUser.subjects);
        multiSelectDropdowns[0]->setSelectedItems(userSubjects);
    }
    
    textBoxes.push_back(new TextBox(350, 270, 300, 45, "Graduation Year:", font));
    if (currentUser.gradYear != 0) {
        textBoxes[0]->setContent(std::to_string(currentUser.gradYear));
    }
    
    createButton(300, 360, 140, 50, "Save Profile", sf::Color(34, 197, 94));
    createButton(450, 360, 140, 50, "Find Groups", sf::Color(168, 85, 247));
    createButton(600, 360, 140, 50, "Logout", sf::Color(239, 68, 68));
    
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
    
    sf::Text subtitleText;
    subtitleText.setFont(font);
    subtitleText.setString("Groups matching your classes");
    subtitleText.setCharacterSize(16);
    subtitleText.setFillColor(sf::Color(160, 160, 160));
    centerText(subtitleText, 100);
    
    createButton(300, 625, 120, 50, "Back", sf::Color(107, 114, 128));
	createButton(580, 625, 140, 50, "Create Group", sf::Color(34, 197, 94));

    // Calculate matches ONCE when entering this page
    cachedMatches = getMatchedGroups();
    
    showMessageText = false;
}

void Application::setupCreateGroupPage() {
    titleText.setFont(font);
    titleText.setString("Create New Study Group");
    titleText.setCharacterSize(36);
    titleText.setFillColor(sf::Color(64, 156, 255));
    titleText.setStyle(sf::Text::Bold);
    centerText(titleText, 60);
    
    // Subtitle
    sf::Text subtitleText;
    subtitleText.setFont(font);
    subtitleText.setString("Start your own study group");
    subtitleText.setCharacterSize(16);
    subtitleText.setFillColor(sf::Color(180, 180, 180));
    centerText(subtitleText, 100);
    
    const float startY = 140;
    const float spacing = 70;
    
    // Group name
    textBoxes.push_back(new TextBox(350, startY, 300, 45, "Group Name:", font));
    
    // Group description
    textBoxes.push_back(new TextBox(350, startY + spacing, 300, 45, "Description:", font));
    
    // Contact information
    textBoxes.push_back(new TextBox(350, startY + 2 * spacing, 300, 45, "Contact (Phone/Email):", font));
    
    // Meeting location
    textBoxes.push_back(new TextBox(350, startY + 3 * spacing, 300, 45, "Meeting Location:", font));
    
    // Subject selection
    std::vector<std::string> subjects = {
        "Mathematics", "Algebra", "Calculus", "Geometry", "Statistics",
        "Science", "Biology", "Chemistry", "Physics", "Environmental Science",
        "English", "Literature", "Creative Writing", "Grammar", "Public Speaking",
        "History", "World History", "US History", "European History", "Government",
        "Computer Science", "Programming", "Data Structures", "Algorithms", "Web Development",
        "Art", "Music", "Theater", "Photography", "Graphic Design",
        "Foreign Languages", "Spanish", "French", "German", "Chinese",
        "Economics", "Psychology", "Sociology", "Philosophy", "Business"
    };
    
    sf::Text subjectLabel;
    subjectLabel.setFont(font);
    subjectLabel.setString("Subjects (select multiple):");
    subjectLabel.setCharacterSize(16);
    subjectLabel.setFillColor(sf::Color::White);
    subjectLabel.setPosition(350, startY + 4 * spacing - 25);
    labels.push_back(subjectLabel);
    
    multiSelectDropdowns.push_back(new MultiSelectDropdown(350, startY + 4 * spacing, 300, 45, font, subjects, 5));
    
    // Buttons
    createButton(350, startY + 5 * spacing + 30, 140, 50, "Create Group", sf::Color(34, 197, 94));
    createButton(510, startY + 5 * spacing + 30, 140, 50, "Cancel", sf::Color(107, 114, 128));
    
    showMessageText = true;
    messageText.setFont(font);
    messageText.setCharacterSize(16);
    messageText.setFillColor(sf::Color(239, 68, 68));
    messageText.setPosition(350, startY + 6 * spacing + 30);
    messageText.setString("");
}

void Application::handleButtonClicks(sf::Vector2i mousePos) {
    // Check if any dropdown is open - if so, don't process button clicks
    for (auto* multiDropdown : multiSelectDropdowns) {
        if (multiDropdown->isDropdownOpen()) {  
            return;  // Don't process button clicks while dropdown is open
        }
    }
    
    // Handle join buttons in group matching page first
    if (currentPage == PageType::GROUP_MATCHING) {
        handleJoinButtonClicks(mousePos);
    }
    
    // Then handle regular buttons
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
        case PageType::CONTACT:
            handleContactButtons(buttonIndex);
            break;
        case PageType::CREATE_GROUP: 
            handleCreateGroupButtons(buttonIndex);
            break;
    }
}

void Application::handleGroupMatchingButtons(size_t buttonIndex) {
    if (buttonIndex == 0) { // Back
        currentPage = PageType::PROFILE;
        setupCurrentPage();
    } else if (buttonIndex == 1) { // Create Group
        currentPage = PageType::CREATE_GROUP;
        setupCurrentPage();
    }
}
void Application::handleCreateGroupButtons(size_t buttonIndex) {
    if (buttonIndex == 0) { // Create Group
        std::string groupName = textBoxes[0]->getContent();
        std::string description = textBoxes[1]->getContent();
        std::string contact = textBoxes[2]->getContent();
        std::string location = textBoxes[3]->getContent();
        std::string subjects = multiSelectDropdowns[0]->getSelectedItemsString();
        
        // Validation
        if (groupName.empty() || description.empty() || contact.empty() || location.empty() || subjects.empty()) {
            messageText.setString("Please fill all fields!");
            return;
        }
        
        // Try to insert into database
        try {
            std::string connStr = "postgresql://postgres:cen3031group4@db.iekosjtwireodvbaqhcm.supabase.co:5432/postgres";
            PgConnector db(connStr);
            
            // FIX: Insert new group into database WITH SUBJECTS
            std::vector<const char*> params = {groupName.c_str(), description.c_str(), subjects.c_str(), contact.c_str()};
            PGresult* res = db.execParams(
                "INSERT INTO groups (group_name, description, subjects, contact) VALUES ($1, $2, $3, $4)",
                params
            );
            PQclear(res);
            
            // Add to local groups vector
            groups.push_back({groupName, description, subjects, location, 1, contact});
            
            std::cout << "Group created successfully: " << groupName << std::endl;
            messageText.setString("Group created successfully!");
            messageText.setFillColor(sf::Color(34, 197, 94));
            
            // Clear form
            for (auto* textBox : textBoxes) {
                textBox->setContent("");
            }
            
        } catch (const std::exception& e) {
            std::cout << "Database insert failed: " << e.what() << std::endl;
            
            // Still add to local groups even if database fails
            groups.push_back({groupName, description, subjects, location, 1, contact});
            messageText.setString("Group created (local only - database error)");
            messageText.setFillColor(sf::Color(255, 165, 0)); // Orange color
        }
        
        // Go back to group matching page
        currentPage = PageType::GROUP_MATCHING;
        setupCurrentPage();
        
    } else if (buttonIndex == 1) { // Cancel
        currentPage = PageType::GROUP_MATCHING;
        setupCurrentPage();
    }
}

void Application::setupContactPage() {
    // Find the selected group in cached matches
    const Group* selectedGroup = nullptr;
    for (const auto& group : cachedMatches) {
        if (group.name == selectedGroupName) {
            selectedGroup = &group;
            break;
        }
    }
    
    if (selectedGroup == nullptr) {
        // Group not found in current matches, go back to group matching
        std::cout << "Group '" << selectedGroupName << "' not found in current matches" << std::endl;
        currentPage = PageType::GROUP_MATCHING;
        setupCurrentPage();
        return;
    }
    
    titleText.setFont(font);
    titleText.setString("Contact Information");
    titleText.setCharacterSize(36);
    titleText.setFillColor(sf::Color(64, 156, 255));
    titleText.setStyle(sf::Text::Bold);
    centerText(titleText, 60);
    
    // Group name
    sf::Text groupNameText;
    groupNameText.setFont(font);
    groupNameText.setString(selectedGroup->name);
    groupNameText.setCharacterSize(28);
    groupNameText.setFillColor(sf::Color(245, 245, 245));
    groupNameText.setStyle(sf::Text::Bold);
    centerText(groupNameText, 120);
    labels.push_back(groupNameText);
    
    // Contact cell label
    sf::Text contactLabel;
    contactLabel.setFont(font);
    contactLabel.setString("Contact Cell:");
    contactLabel.setCharacterSize(20);
    contactLabel.setFillColor(sf::Color(180, 180, 180));
    contactLabel.setPosition(350, 200);
    labels.push_back(contactLabel);
    
    // Contact cell number
    sf::Text contactNumber;
    contactNumber.setFont(font);
    contactNumber.setString(selectedGroup->contactCell);
    contactNumber.setCharacterSize(24);
    contactNumber.setFillColor(sf::Color(34, 197, 94));
    contactNumber.setStyle(sf::Text::Bold);
    contactNumber.setPosition(350, 240);
    labels.push_back(contactNumber);
    
    // Group description
    sf::Text descLabel;
    descLabel.setFont(font);
    descLabel.setString("About this group:");
    descLabel.setCharacterSize(18);
    descLabel.setFillColor(sf::Color(180, 180, 180));
    descLabel.setPosition(350, 300);
    labels.push_back(descLabel);
    
    sf::Text description;
    description.setFont(font);
    description.setString(selectedGroup->description);
    description.setCharacterSize(16);
    description.setFillColor(sf::Color(200, 200, 200));
    description.setPosition(350, 330);
    labels.push_back(description);
    
    // Location
    sf::Text locationLabel;
    locationLabel.setFont(font);
    locationLabel.setString("Meeting Location:");
    locationLabel.setCharacterSize(18);
    locationLabel.setFillColor(sf::Color(180, 180, 180));
    locationLabel.setPosition(350, 380);
    labels.push_back(locationLabel);
    
    sf::Text location;
    location.setFont(font);
    location.setString(selectedGroup->location);
    location.setCharacterSize(16);
    location.setFillColor(sf::Color(200, 200, 200));
    location.setPosition(350, 410);
    labels.push_back(location);
    
    createButton(450, 550, 100, 50, "Back", sf::Color(107, 114, 128));
    
    showMessageText = false;
}

void Application::createButton(float x, float y, float width, float height, const std::string& text, sf::Color color) {
    sf::RectangleShape button(sf::Vector2f(width, height));
    button.setPosition(x, y);
    button.setFillColor(color);
    button.setOutlineThickness(0);
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
    text.setPosition((1000 - bounds.width) / 2.0f, y);
}

void Application::handleEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();
        
        for (auto* textBox : textBoxes) {
            textBox->handleEvent(event);
        }
        
        for (auto* dropdown : dropdownMenus) {
            dropdown->handleEvent(event, window);
        }
        
        for (auto* multiDropdown : multiSelectDropdowns) {
            multiDropdown->handleEvent(event, window);
        }
        
        if (event.type == sf::Event::MouseButtonPressed) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            handleButtonClicks(mousePos);
        }
    }
}


void Application::handleJoinButtonClicks(sf::Vector2i mousePos) {
    // Use cached matches instead of recalculating
    float yPos = 140;
    
    for (size_t i = 0; i < cachedMatches.size() && i < 6; ++i) {
        sf::FloatRect joinButtonBounds(710, yPos + 20, 120, 30);
        if (joinButtonBounds.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
            selectedGroupName = cachedMatches[i].name;  // Use cached matches
            std::cout << "Join button clicked for group: " << selectedGroupName << std::endl;
            currentPage = PageType::CONTACT;
            setupCurrentPage();
            return;
        }
        yPos += 80;
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
                std::cout << "Database login successful for: " << username << std::endl;
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
        std::string currentClasses = multiSelectDropdowns[0]->getSelectedItemsString();
        std::string username = textBoxes[2]->getContent();
        std::string password = textBoxes[3]->getContent();
        std::string confirmPass = textBoxes[4]->getContent();
        
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
            
            // Insert into profile table
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
            
            // Insert into user_login
            std::vector<const char*> loginParams = {username.c_str(), password.c_str(), profileId.c_str()};
            PGresult* loginRes = db.execParams(
                "INSERT INTO user_login (username, password_hash, profile_id) VALUES ($1, $2, $3)",
                loginParams
            );
            PQclear(loginRes);
            
            PGresult* commitRes = db.exec("COMMIT");
            PQclear(commitRes);
            
            // Set current user and login
            currentUser.name = name;
            currentUser.username = username;
            currentUser.subjects = currentClasses;
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
        std::string subjects = multiSelectDropdowns[0]->getSelectedItemsString();
        std::string gradYearStr = textBoxes[0]->getContent();
        
        if (subjects.empty()) {
            messageText.setString("Please select at least one subject!");
            return;
        }
        
        int gradYear = 0;
        try {
            gradYear = std::stoi(gradYearStr);
        } catch (...) {
            messageText.setString("Invalid graduation year!");
            return;
        }
        
        // Update local user data
        currentUser.subjects = subjects;
        currentUser.gradYear = gradYear;
        
        messageText.setString("Profile saved successfully!");
        std::cout << "Profile updated" << std::endl;
        
    } else if (buttonIndex == 1) { // Find Groups
        if (currentUser.subjects.empty()) {
            messageText.setString("Please add subjects first!");
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


void Application::handleContactButtons(size_t buttonIndex) {
    if (buttonIndex == 0) { // Back
        currentPage = PageType::GROUP_MATCHING;
        setupCurrentPage();
    }
}

std::vector<Group> Application::getMatchedGroups() {
    std::vector<Group> matches;
    std::vector<std::string> userSubjects = parseSubjects(currentUser.subjects);
    
    std::cout << "=== GROUP MATCHING DEBUG ===" << std::endl;
    std::cout << "User subjects (" << userSubjects.size() << "): ";
    for (const auto& subject : userSubjects) {
        std::cout << "'" << subject << "' ";
    }
    std::cout << std::endl;
    std::cout << "Total groups available: " << groups.size() << std::endl;
    
    for (size_t groupIndex = 0; groupIndex < groups.size(); groupIndex++) {
        const auto& group = groups[groupIndex];
        std::vector<std::string> groupSubjects = parseSubjects(group.subjects);
        
        std::cout << "\n--- Group " << (groupIndex + 1) << ": " << group.name << " ---" << std::endl;
        std::cout << "Raw subjects: '" << group.subjects << "'" << std::endl;
        std::cout << "Parsed subjects (" << groupSubjects.size() << "): ";
        for (const auto& subject : groupSubjects) {
            std::cout << "'" << subject << "' ";
        }
        std::cout << std::endl;
        
        // Check for at least one common subject
        bool hasMatch = false;
        std::string matchedSubject = "";
        
        for (const auto& userSubject : userSubjects) {
            if (hasMatch) break; // Already found a match, no need to continue
            
            for (const auto& groupSubject : groupSubjects) {
                // Case-insensitive comparison
                std::string userLower = userSubject;
                std::string groupLower = groupSubject;
                std::transform(userLower.begin(), userLower.end(), userLower.begin(), ::tolower);
                std::transform(groupLower.begin(), groupLower.end(), groupLower.begin(), ::tolower);
                
                if (userLower == groupLower) {
                    hasMatch = true;
                    matchedSubject = userSubject + " == " + groupSubject;
                    std::cout << "  ✅ MATCH: " << matchedSubject << std::endl;
                    break;
                }
            }
        }
        
        if (hasMatch) {
            matches.push_back(group);
            std::cout << "  *** ADDED TO MATCHES *** (Total so far: " << matches.size() << ")" << std::endl;
        } else {
            std::cout << "  ❌ No common subjects found" << std::endl;
        }
    }
    
    std::cout << "\n=== FINAL RESULTS ===" << std::endl;
    std::cout << "Total matches found: " << matches.size() << std::endl;
    for (size_t i = 0; i < matches.size(); i++) {
        std::cout << "  " << (i+1) << ". " << matches[i].name << std::endl;
    }
    std::cout << "===================" << std::endl;
    
    return matches;
}

void Application::update() {
    for (auto* textBox : textBoxes) {
        textBox->update();
    }
    
    for (auto* dropdown : dropdownMenus) {
        dropdown->update();
    }
    
    for (auto* multiDropdown : multiSelectDropdowns) {
        multiDropdown->update();
    }
}

void Application::render() {
    window.clear(sf::Color(15, 23, 42));
    
    window.draw(titleText);
    
    for (const auto& label : labels) {
        window.draw(label);
    }
    
    for (auto* textBox : textBoxes) {
        textBox->draw(window);
    }
    
    // Draw buttons before dropdowns so dropdowns appear on top
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
    
    // Draw dropdowns LAST so they appear on top of everything else
    for (auto* dropdown : dropdownMenus) {
        dropdown->draw(window);
    }
    
    for (auto* multiDropdown : multiSelectDropdowns) {
        multiDropdown->draw(window);
    }
    
    window.display();
}

void Application::renderGroupMatches() {

   float yPos = 140;
	std::cout << "RENDERING: cachedMatches has " << cachedMatches.size() << " groups" << std::endl;

   for (size_t i = 0; i < cachedMatches.size() && i < 6; ++i) {
        const auto& group = cachedMatches[i];
        std::cout << "Rendering group " << (i+1) << ": " << group.name << " at yPos=" << yPos << std::endl;

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
        locationText.setString(group.location);
        locationText.setCharacterSize(12);
        locationText.setFillColor(sf::Color(107, 114, 128));
        locationText.setPosition(120, yPos + 52);
        window.draw(locationText);
        
		// Join button - make it wider
		sf::RectangleShape joinButton(sf::Vector2f(120, 30));  // Changed from 80 to 120
		joinButton.setPosition(710, yPos + 20);  // Moved left to fit (was 750)
		joinButton.setFillColor(sf::Color(168, 85, 247)); // Purple color
		window.draw(joinButton);

		sf::Text joinButtonText;
		joinButtonText.setFont(font);
		joinButtonText.setString("See Information");
		joinButtonText.setCharacterSize(11);  // Slightly smaller font
		joinButtonText.setFillColor(sf::Color::White);
		joinButtonText.setStyle(sf::Text::Bold);

		// Center the text in the wider button
		sf::FloatRect textBounds = joinButtonText.getLocalBounds();
		joinButtonText.setPosition(
			710 + (120 - textBounds.width) / 2.0f,  // Center in 120px button
			yPos + 20 + (30 - textBounds.height) / 2.0f
		);
		
		window.draw(joinButtonText);
		yPos += 80;	
    }
    
	if (cachedMatches.empty()) {
        sf::Text noMatches;
        noMatches.setFont(font);
        noMatches.setString("No matching study groups found yet.\nTry updating your classes in your profile!");
        noMatches.setCharacterSize(18);
        noMatches.setFillColor(sf::Color(156, 163, 175));
        centerText(noMatches, 300);
        window.draw(noMatches);
    }
	
}
