#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <memory>
#include "TextBox.h"
#include "Dropdown.h"
#include "MultiSelectDropdown.h"

enum class PageType {
    LOGIN,
    REGISTRATION,
    PROFILE,
    GROUP_MATCHING,
    CONTACT
};

struct User {
    std::string username;
    std::string name;
    std::string subjects;
    int gradYear = 0;
    int profileId = 0;
};

struct Group {
    std::string name;
    std::string description;
    std::string subjects;
    std::string location;
	
    int memberCount;
    std::string contactCell;
    
    // Constructor for backward compatibility
    Group(const std::string& n, const std::string& d, const std::string& s, 
          const std::string& l, int m, const std::string& c = "No contact available")
        : name(n), description(d), subjects(s), location(l), memberCount(m), contactCell(c) {}
    
    // Default constructor
    Group() : memberCount(0), contactCell("No contact available") {}
};

class Application {
private:
    sf::RenderWindow window;
    sf::Font font;
    PageType currentPage;
    bool isLoggedIn;
    User currentUser;
    
    // UI elements
    std::vector<TextBox*> textBoxes;
    std::vector<Dropdown*> dropdownMenus;
    std::vector<MultiSelectDropdown*> multiSelectDropdowns;
    std::vector<sf::RectangleShape> buttons;
    std::vector<sf::Text> buttonTexts;
    std::vector<sf::Text> labels;
    
    sf::Text titleText;
    sf::Text messageText;
    bool showMessageText;
    
    // Data
    std::vector<Group> groups;
    int selectedGroupIndex;
    
    // Private methods
    void initializeDatabase();
    void setupCurrentPage();
    void setupLoginPage();
    void setupRegistrationPage();
    void setupProfilePage();
    void setupGroupMatchingPage();
    void setupContactPage();
    void clearTextBoxes();
    void createButton(float x, float y, float width, float height, const std::string& text, sf::Color color);
    void centerText(sf::Text& text, float y);
    void handleEvents();
    void handleButtonClicks(sf::Vector2i mousePos);
    void handleJoinButtonClicks(sf::Vector2i mousePos);
    void handleButtonClick(size_t buttonIndex);
    void handleLoginButtons(size_t buttonIndex);
    void handleRegistrationButtons(size_t buttonIndex);
    void handleProfileButtons(size_t buttonIndex);
    void handleGroupMatchingButtons(size_t buttonIndex);
    void handleContactButtons(size_t buttonIndex);
    void update();
    void render();
    void renderGroupMatches();
    std::vector<Group> getMatchedGroups();
    std::vector<std::string> parseSubjects(const std::string& subjects);

public:
    Application();
    ~Application();
    void run();
};
