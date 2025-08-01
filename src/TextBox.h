#ifndef TEXTBOX_H
#define TEXTBOX_H
#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>

class TextBox {
private:
    sf::RectangleShape box;
    sf::Text text;
    sf::Text label;
    std::string content;
    std::string displayContent;  // What to show (with asterisks if password)
    bool isActive;
    bool showCursor;
    bool isPassword;  // New member for password mode
    sf::Clock cursorClock;
    
public:
    TextBox(float x, float y, float width, float height, const std::string& labelText, sf::Font& font);
    void handleEvent(const sf::Event& event);
    void update();
    void updateDisplayText();
    void draw(sf::RenderWindow& window);
    std::string getContent() const;
    void setActive(bool active);
    void setContent(const std::string& content);
    void setPasswordMode(bool passwordMode);
    bool getIsActive() const { return isActive; }
};

#endif
