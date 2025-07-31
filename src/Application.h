#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <memory>
#include "TextBox.h"
#include "DatabaseManager.h"

enum class PageType {
    LOGIN,
    REGISTRATION,
    PROFILE,
    GROUP_MATCHING
};

class Application {
public:
    Application();
    ~Application();
    void run();

private:
    sf::RenderWindow window;
    sf::Font font;
    sf::Text titleText;
    sf::Text messageText;
    bool showMessageText;
    
    PageType currentPage;
    bool isLoggedIn;
    
    std::vector<TextBox*> textBoxes;
    std::vector<sf::RectangleShape> buttons;
    std::vector<sf::Text> buttonTexts;
    std::vector<sf::Text> labels;
    
    // Database integration
    std::unique_ptr<DatabaseManager> dbManager;
    User currentUser;
    std::vector<Group> groups;
    
    // Initialize database connection
    void initializeDatabase();
    
    void clearTextBoxes();
    void setupCurrentPage();
    void setupLoginPage();
    void setupRegistrationPage();
    void setupProfilePage();
    void setupGroupMatchingPage();
    
    void createButton(float x, float y, float width, float height, const std::string& text, sf::Color color);
    void centerText(sf::Text& text, float y);
    
    void handleEvents();
    void handleButtonClicks(sf::Vector2i mousePos);
    void handleButtonClick(size_t buttonIndex);
    void handleLoginButtons(size_t buttonIndex);
    void handleRegistrationButtons(size_t buttonIndex);
    void handleProfileButtons(size_t buttonIndex);
    void handleGroupMatchingButtons(size_t buttonIndex);
    
    std::vector<Group> getMatchedGroups();
    
    void update();
    void render();
    void renderGroupMatches();
    
    // Helper method to parse subjects
    std::vector<std::string> parseSubjects(const std::string& subjects);
};