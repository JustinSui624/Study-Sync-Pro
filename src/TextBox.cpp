#include "TextBox.h"


TextBox::TextBox(float x, float y, float width, float height, const std::string& labelText, sf::Font& font)
{
    // Setup the box
    box.setSize(sf::Vector2f(width, height));
    box.setPosition(x, y);
    box.setFillColor(sf::Color(50, 50, 50));
    box.setOutlineThickness(2);
    box.setOutlineColor(sf::Color::White);
        
    // Setup the label
    label.setFont(font);
    label.setString(labelText);
    label.setCharacterSize(16);
    label.setFillColor(sf::Color::White);
    label.setPosition(x, y - 25);
        
    // Setup the text
    text.setFont(font);
    text.setCharacterSize(16);
    text.setFillColor(sf::Color::White);
    text.setPosition(x + 5, y + 5);
        
    isActive = false;
    showCursor = true;
    content = "";
}
void TextBox::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2i mousePos = sf::Mouse::getPosition();
        sf::FloatRect boxBounds = box.getGlobalBounds();
        
        // Check if clicked inside the text box
        if (boxBounds.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
            isActive = true;
            box.setOutlineColor(sf::Color::Blue);
        } else {
            isActive = false;
            box.setOutlineColor(sf::Color::White);
        }
    }
    
    if (isActive && event.type == sf::Event::TextEntered) {
        if (event.text.unicode == 8) { // Backspace
            if (!content.empty()) {
                content.pop_back();
            }
        } else if (event.text.unicode >= 32 && event.text.unicode < 127) { // Printable characters
            content += static_cast<char>(event.text.unicode);
        }
        
        updateDisplayText();
    }
}

void TextBox::update() {
    // Cursor blinking
    if (cursorClock.getElapsedTime().asMilliseconds() > 500) {
        showCursor = !showCursor;
        cursorClock.restart();
    }
    
    if (isActive) {
        updateDisplayText();
    }
}

void TextBox::updateDisplayText() {
    std::string displayText = content;
    if (isActive && showCursor) {
        displayText += "|";
    }
    text.setString(displayText);
}

void TextBox::draw(sf::RenderWindow& window) {
    window.draw(box);
    window.draw(label);
    window.draw(text);
}

std::string TextBox::getContent() const {
    return content;
}

void TextBox::setActive(bool active) {
    isActive = active;
    box.setOutlineColor(active ? sf::Color::Blue : sf::Color::White);
}