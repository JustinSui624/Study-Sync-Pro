#ifndef TEXTBOX_H
#define TEXTBOX_H

#include <SFML/Graphics.hpp>

class TextBox {
private:
    sf::RectangleShape box;
    sf::Text text;
    sf::Text label;
    std::string content;
    bool isActive;
    bool showCursor;
    sf::Clock cursorClock;
    
public:
    TextBox(float x, float y, float width, float height, const std::string& labelText, sf::Font& font);
    void handleEvent(const sf::Event& event);
    void update();
    void updateDisplayText();
    void draw(sf::RenderWindow& window);
    std::string getContent() const;
    void setActive(bool active);
};

#endif