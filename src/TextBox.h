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
    sf::String labelText;  
    sf::Font& font;        
    sf::Vector2f position; 
    sf::Vector2f size;     
    std::string content;
    bool isActive;
    bool showCursor;
    bool isPassword;
    sf::Clock cursorClock;
    
public:
    TextBox(float x, float y, float width, float height, const std::string& label, sf::Font& font, bool password = false);
    void handleEvent(const sf::Event& event);
    void update();
    void updateDisplayText();
    void draw(sf::RenderWindow& window);
    std::string getContent() const;
    void setActive(bool active);
    void setContent(const std::string& content);
};
#endif
