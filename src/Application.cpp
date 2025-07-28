#include "Application.h"

Application::Application() : window(sf::VideoMode(800, 600), "Study Group Finder"), 
                            currentPage(PageType::LOGIN), isLoggedIn(false) {
    if (!font.loadFromFile("./fonts/arial.ttf")) {
        std::cout << "Error loading font!" << std::endl;
    }
    
    // Initialize some sample groups
    initializeSampleGroups();
    setupCurrentPage();
}

Application::~Application() {
    clearTextBoxes();
}

void Application::run() {
    while (window.isOpen()) {
        handleEvents();
        update();
        render();
    }
}

void Application::initializeSampleGroups() {
    // STEM Groups
    groups.push_back({"Advanced Calculus Study Circle", "Tackle multivariable calculus and differential equations", "calculus,mathematics,derivatives,integrals", "MIT", 8});
    groups.push_back({"Linear Algebra Workshop", "Matrix operations and vector spaces", "linear algebra,mathematics,matrices,vectors", "Stanford University", 12});
    groups.push_back({"Organic Chemistry Lab", "Synthesis and reaction mechanisms", "chemistry,organic chemistry,lab,reactions", "Harvard University", 10});
    groups.push_back({"Physics Problem Solvers", "Classical and quantum mechanics", "physics,mechanics,quantum,thermodynamics", "Caltech", 7});
    groups.push_back({"Data Structures & Algorithms", "Coding interview prep and theory", "computer science,algorithms,programming,java,python", "UC Berkeley", 15});
    groups.push_back({"Molecular Biology Research", "Genetics and cell biology", "biology,genetics,molecular biology,cell biology", "Yale University", 9});
    groups.push_back({"Statistics & Data Analysis", "Statistical modeling and R programming", "statistics,data science,R,probability", "University of Chicago", 11});
    groups.push_back({"Engineering Mechanics", "Statics and dynamics problems", "engineering,mechanics,statics,dynamics", "Georgia Tech", 13});
    
    // Liberal Arts Groups
    groups.push_back({"Shakespeare Analysis Group", "Close reading of major works", "literature,english,shakespeare,poetry", "Columbia University", 8});
    groups.push_back({"World History Seminar", "From ancient civilizations to modern times", "history,world history,ancient history,modern history", "Princeton University", 10});
    groups.push_back({"Philosophy Discussion Circle", "Ethics, logic, and metaphysics", "philosophy,ethics,logic,metaphysics", "University of Pennsylvania", 7});
    groups.push_back({"Creative Writing Workshop", "Fiction, poetry, and screenwriting", "writing,creative writing,poetry,fiction", "NYU", 12});
    groups.push_back({"Art History Study Group", "Renaissance to contemporary movements", "art history,renaissance,modern art,contemporary art", "University of Michigan", 9});
    groups.push_back({"Foreign Language Exchange", "Spanish, French, and German practice", "spanish,french,german,linguistics,language", "UCLA", 14});
    
    // Business & Economics Groups
    groups.push_back({"Microeconomics Study Session", "Supply, demand, and market analysis", "economics,microeconomics,markets,business", "Wharton School", 11});
    groups.push_back({"Corporate Finance Workshop", "Valuation and investment strategies", "finance,corporate finance,investment,accounting", "University of Texas", 8});
    groups.push_back({"Marketing Strategy Group", "Digital marketing and consumer behavior", "marketing,business,strategy,digital marketing", "Northwestern University", 10});
    groups.push_back({"International Business Seminar", "Global trade and cultural studies", "business,international business,trade,culture", "Georgetown University", 9});
    
    // Social Sciences Groups
    groups.push_back({"Psychology Research Methods", "Experimental design and statistics", "psychology,research methods,statistics,experiments", "University of Wisconsin", 12});
    groups.push_back({"Sociology Theory Discussion", "Classical and contemporary theorists", "sociology,social theory,research,society", "University of California San Diego", 8});
    groups.push_back({"Political Science Debate Club", "Current events and policy analysis", "political science,government,policy,debate", "George Washington University", 13});
    groups.push_back({"Anthropology Field Methods", "Ethnography and cultural analysis", "anthropology,culture,ethnography,research", "University of Arizona", 7});
    
    // Pre-Professional Groups
    groups.push_back({"MCAT Prep Intensive", "Biology, chemistry, and physics review", "MCAT,biology,chemistry,physics,pre-med", "Johns Hopkins University", 15});
    groups.push_back({"LSAT Logic Games", "Analytical reasoning practice", "LSAT,logic,pre-law,reasoning,law school", "Duke University", 10});
    groups.push_back({"GRE Quantitative Prep", "Math and analytical writing", "GRE,mathematics,analytical writing,graduate school", "University of Virginia", 11});
    
    // Specialized Fields
    groups.push_back({"Environmental Science Lab", "Ecology and sustainability studies", "environmental science,ecology,sustainability,climate", "University of Colorado Boulder", 9});
    groups.push_back({"Public Health Research", "Epidemiology and health policy", "public health,epidemiology,health policy,statistics", "Emory University", 8});
    groups.push_back({"Music Theory & Composition", "Harmony, counterpoint, and analysis", "music,music theory,composition,harmony", "Juilliard School", 6});
    groups.push_back({"Architecture Design Studio", "Spatial design and technical drawing", "architecture,design,drawing,spatial design", "MIT", 7});
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
    // Title
    titleText.setFont(font);
    titleText.setString("Login");
    titleText.setCharacterSize(32);
    titleText.setFillColor(sf::Color::White);
    titleText.setStyle(sf::Text::Bold);
    centerText(titleText, 50);
    
    // Text boxes
    textBoxes.push_back(new TextBox(250, 150, 300, 30, "Username:", font));
    textBoxes.push_back(new TextBox(250, 220, 300, 30, "Password:", font));
    
    // Buttons
    createButton(325, 300, 150, 40, "Login", sf::Color::Blue);
    createButton(325, 360, 150, 40, "Register", sf::Color::Green);
    
    // Message text
    messageText.setFont(font);
    messageText.setCharacterSize(16);
    messageText.setFillColor(sf::Color::Red);
    messageText.setPosition(250, 420);
}

void Application::setupRegistrationPage() {
    titleText.setFont(font);
    titleText.setString("Registration");
    titleText.setCharacterSize(32);
    titleText.setFillColor(sf::Color::White);
    titleText.setStyle(sf::Text::Bold);
    centerText(titleText, 30);
    
    textBoxes.push_back(new TextBox(250, 100, 300, 30, "Username:", font));
    textBoxes.push_back(new TextBox(250, 150, 300, 30, "Email:", font));
    textBoxes.push_back(new TextBox(250, 200, 300, 30, "Password:", font));
    textBoxes.push_back(new TextBox(250, 250, 300, 30, "Confirm Password:", font));
    textBoxes.push_back(new TextBox(250, 300, 300, 30, "Full Name:", font));
    textBoxes.push_back(new TextBox(250, 350, 300, 30, "Age:", font));
    
    createButton(275, 420, 100, 40, "Register", sf::Color::Blue);
    createButton(425, 420, 100, 40, "Back", sf::Color(128, 128, 128));
    
    messageText.setFont(font);
    messageText.setCharacterSize(16);
    messageText.setFillColor(sf::Color::Red);
    messageText.setPosition(250, 480);
}

void Application::setupProfilePage() {
    titleText.setFont(font);
    titleText.setString("Profile - " + currentUser.username);
    titleText.setCharacterSize(28);
    titleText.setFillColor(sf::Color::White);
    titleText.setStyle(sf::Text::Bold);
    centerText(titleText, 30);
    
    textBoxes.push_back(new TextBox(250, 100, 300, 30, "Subjects/Topics (comma separated):", font));
    textBoxes.push_back(new TextBox(250, 150, 300, 30, "School/University:", font));
    
    // Pre-fill with existing data
    if (!currentUser.interests.empty()) {
        textBoxes[0]->setContent(currentUser.interests);
    }
    if (!currentUser.location.empty()) {
        textBoxes[1]->setContent(currentUser.location);
    }
    
    createButton(250, 220, 120, 40, "Save Profile", sf::Color::Blue);
    createButton(380, 220, 120, 40, "Find Groups", sf::Color::Green);
    createButton(510, 220, 90, 40, "Logout", sf::Color::Red);
    
    messageText.setFont(font);
    messageText.setCharacterSize(16);
    messageText.setFillColor(sf::Color::Green);
    messageText.setPosition(250, 280);
}

void Application::setupGroupMatchingPage() {
    titleText.setFont(font);
    titleText.setString("Recommended Study Groups");
    titleText.setCharacterSize(28);
    titleText.setFillColor(sf::Color::White);
    titleText.setStyle(sf::Text::Bold);
    centerText(titleText, 30);
    
    createButton(350, 500, 100, 40, "Back", sf::Color(128, 128, 128));
    
    // Display matched groups (will be rendered separately)
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
        
        // Handle text box events
        for (auto* textBox : textBoxes) {
            textBox->handleEvent(event);
        }
        
        // Handle button clicks
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
        
        if (users.find(username) != users.end() && users[username].password == password) {
            currentUser = users[username];
            isLoggedIn = true;
            currentPage = PageType::PROFILE;
            setupCurrentPage();
            messageText.setString("");
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
        std::string username = textBoxes[0]->getContent();
        std::string email = textBoxes[1]->getContent();
        std::string password = textBoxes[2]->getContent();
        std::string confirmPass = textBoxes[3]->getContent();
        std::string name = textBoxes[4]->getContent();
        std::string ageStr = textBoxes[5]->getContent();
        
        if (username.empty() || email.empty() || password.empty() || name.empty()) {
            messageText.setString("Please fill all fields!");
            return;
        }
        
        if (password != confirmPass) {
            messageText.setString("Passwords don't match!");
            return;
        }
        
        if (users.find(username) != users.end()) {
            messageText.setString("Username already exists!");
            return;
        }
        
        int age = 0;
        try {
            age = std::stoi(ageStr);
        } catch (...) {
            messageText.setString("Invalid age!");
            return;
        }
        
        // Create new user
        User newUser;
        newUser.username = username;
        newUser.email = email;
        newUser.password = password;
        newUser.name = name;
        newUser.age = age;
        
        users[username] = newUser;
        currentUser = newUser;
        isLoggedIn = true;
        currentPage = PageType::PROFILE;
        setupCurrentPage();
        
    } else if (buttonIndex == 1) { // Back
        currentPage = PageType::LOGIN;
        setupCurrentPage();
    }
}

void Application::handleProfileButtons(size_t buttonIndex) {
    if (buttonIndex == 0) { // Save Profile
        currentUser.interests = textBoxes[0]->getContent();
        currentUser.location = textBoxes[1]->getContent();
        users[currentUser.username] = currentUser;
        messageText.setString("Profile saved successfully!");
        
    } else if (buttonIndex == 1) { // Find Groups
        if (currentUser.interests.empty()) {
            messageText.setString("Please add interests first!");
        } else {
            currentPage = PageType::GROUP_MATCHING;
            setupCurrentPage();
        }
    } else if (buttonIndex == 2) { // Logout
        isLoggedIn = false;
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
    std::vector<std::string> userInterests;
    
    // Parse user interests (now subjects)
    std::string interests = currentUser.interests;
    std::string delimiter = ",";
    size_t pos = 0;
    std::string token;
    while ((pos = interests.find(delimiter)) != std::string::npos) {
        token = interests.substr(0, pos);
        // Remove spaces
        token.erase(std::remove_if(token.begin(), token.end(), ::isspace), token.end());
        userInterests.push_back(token);
        interests.erase(0, pos + delimiter.length());
    }
    if (!interests.empty()) {
        interests.erase(std::remove_if(interests.begin(), interests.end(), ::isspace), interests.end());
        userInterests.push_back(interests);
    }
    
    // Find matching groups
    for (const auto& group : groups) {
        for (const auto& userInterest : userInterests) {
            if (group.interests.find(userInterest) != std::string::npos) {
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
    window.clear(sf::Color::Black);
    
    // Draw title
    window.draw(titleText);
    
    // Draw text boxes
    for (auto* textBox : textBoxes) {
        textBox->draw(window);
    }
    
    // Draw buttons
    for (size_t i = 0; i < buttons.size(); ++i) {
        window.draw(buttons[i]);
        window.draw(buttonTexts[i]);
    }
    
    // Draw message
    window.draw(messageText);
    
    // Special rendering for group matching page
    if (currentPage == PageType::GROUP_MATCHING) {
        renderGroupMatches();
    }
    
    window.display();
}

void Application::renderGroupMatches() {
    auto matches = getMatchedGroups();
    float yPos = 80;
    
    for (size_t i = 0; i < matches.size() && i < 5; ++i) {
        const auto& group = matches[i];
        
        // Group name
        sf::Text groupName;
        groupName.setFont(font);
        groupName.setString(group.name + " (" + std::to_string(group.memberCount) + " members)");
        groupName.setCharacterSize(18);
        groupName.setFillColor(sf::Color::Yellow);
        groupName.setPosition(50, yPos);
        window.draw(groupName);
        
        // Group description
        sf::Text groupDesc;
        groupDesc.setFont(font);
        groupDesc.setString(group.description);
        groupDesc.setCharacterSize(14);
        groupDesc.setFillColor(sf::Color::White);
        groupDesc.setPosition(70, yPos + 25);
        window.draw(groupDesc);
        
        // Location and subjects
        sf::Text groupInfo;
        groupInfo.setFont(font);
        groupInfo.setString("School: " + group.location + " | Subjects: " + group.interests);
        groupInfo.setCharacterSize(12);
        groupInfo.setFillColor(sf::Color::Cyan);
        groupInfo.setPosition(70, yPos + 45);
        window.draw(groupInfo);
        
        yPos += 80;
    }
    
    if (matches.empty()) {
        sf::Text noMatches;
        noMatches.setFont(font);
        noMatches.setString("No matching study groups found. Try updating your subjects!");
        noMatches.setCharacterSize(16);
        noMatches.setFillColor(sf::Color::Red);
        noMatches.setPosition(150, 150);
        window.draw(noMatches);
    }
}