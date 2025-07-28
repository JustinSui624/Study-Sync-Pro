#ifndef APPLICATION_H
#define APPLICATION_H

#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include "TextBox.h"

enum class PageType {
    LOGIN,
    REGISTRATION,
    PROFILE,
    GROUP_MATCHING
};

struct User {
    std::string username;
    std::string email;
    std::string password;
    std::string name;
    int age;
    std::string interests;
    std::string location;
};

struct Group {
    std::string name;
    std::string description;
    std::string interests;
    std::string location;
    int memberCount;
};

class Application {
private:
    sf::RenderWindow window;
    sf::Font font;
    PageType currentPage;
    std::map<std::string, User> users;
    std::vector<Group> groups;
    User currentUser;
    bool isLoggedIn;
    
    // UI Elements
    std::vector<TextBox*> textBoxes;
    std::vector<sf::RectangleShape> buttons;
    std::vector<sf::Text> buttonTexts;
    std::vector<sf::Text> labels;
    sf::Text titleText;
    sf::Text messageText;
    
public:
    Application();
    ~Application();
    void run();
    
private:
    // Initialization methods
    void initializeSampleGroups();
    void clearTextBoxes();
    void setupCurrentPage();
    
    // Page setup methods
    void setupLoginPage();
    void setupRegistrationPage();
    void setupProfilePage();
    void setupGroupMatchingPage();
    
    // UI helper methods
    void createButton(float x, float y, float width, float height, const std::string& text, sf::Color color);
    void centerText(sf::Text& text, float y);
    
    // Event handling methods
    void handleEvents();
    void handleButtonClicks(sf::Vector2i mousePos);
    void handleButtonClick(size_t buttonIndex);
    void handleLoginButtons(size_t buttonIndex);
    void handleRegistrationButtons(size_t buttonIndex);
    void handleProfileButtons(size_t buttonIndex);
    void handleGroupMatchingButtons(size_t buttonIndex);
    
    // Group matching methods
    std::vector<Group> getMatchedGroups();
    
    // Rendering methods
    void update();
    void render();
    void renderGroupMatches();
};

#endif