#include "TextBox.h"
#include <iostream>

TextBox::TextBox(float x, float y, float width, float height, const std::string& label, sf::Font& font, bool password)
    : position(x, y), size(width, height), labelText(label), font(font), 
      isActive(false), isPassword(password) {  // Remove the extra {
    
    // Setup the box
    this->box.setSize(sf::Vector2f(width, height));
    this->box.setPosition(x, y);
    this->box.setFillColor(sf::Color(50, 50, 50));
    this->box.setOutlineThickness(2);
    this->box.setOutlineColor(sf::Color::White);
        
    // Setup the label
    this->label.setFont(font);
    this->label.setString(labelText);
    this->label.setCharacterSize(16);
    this->label.setFillColor(sf::Color::White);
    this->label.setPosition(x, y - 25);
        
    // Setup the text
    this->text.setFont(font);
    this->text.setCharacterSize(16);
    this->text.setFillColor(sf::Color::White);
    this->text.setPosition(x + 5, y + 5);
        
    this->isActive = false;
    this->showCursor = true;
    this->content = "";
}

void TextBox::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
        sf::FloatRect boxBounds = box.getGlobalBounds();
        
        // Check if clicked inside the text box
        if (boxBounds.contains(mousePos)) {
            this->isActive = true;
            this->box.setOutlineColor(sf::Color::Blue);
        } else {
            this->isActive = false;
            this->box.setOutlineColor(sf::Color::White);
        }
    }
    
    if (this->isActive && event.type == sf::Event::TextEntered) {
        if (event.text.unicode == 8) { // Backspace
            if (!this->content.empty()) {
                this->content.pop_back();
            }
        } else if (event.text.unicode >= 32 && event.text.unicode < 127) { // Printable characters
            this->content += static_cast<char>(event.text.unicode);
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
    
    if (this->isActive) {
        updateDisplayText();
    }
}

void TextBox::updateDisplayText() {
    std::string displayText;
    
    if (isPassword && !content.empty()) {
        // Show asterisks for password
        displayText = std::string(content.length(), '*');
    } else {
        // Show normal text
        displayText = content;
    }
    
    // Add cursor if active
    if (this->isActive && this->showCursor) {
        displayText += "|";
    }
    
    this->text.setString(displayText);
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
    this->isActive = active;
    this->box.setOutlineColor(active ? sf::Color::Blue : sf::Color::White);
}

void TextBox::setContent(const std::string& content) {
    this->content = content;
    updateDisplayText();
}
