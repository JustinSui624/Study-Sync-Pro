#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include "TextBox.h"

int main()
{
    // create the window
    sf::RenderWindow window(sf::VideoMode(800, 600), "Registration Page");
    
    sf::Text titleText;
    sf::Font font;
    if (!font.loadFromFile("./fonts/arial.ttf"))
    {
        std::cout << "Error loading font!" << std::endl;
        return -1;
    }
    
    // Setup title
    titleText.setFont(font);
    titleText.setString("Registration Page");
    int font_size = 32;
    titleText.setCharacterSize(font_size);
    titleText.setFillColor(sf::Color::White);
    titleText.setStyle(sf::Text::Bold | sf::Text::Underlined);
    
    // Center the title
    sf::FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setPosition(
        (800 - titleBounds.width) / 2.0f,
        50
    );
    
    // Create text boxes
    TextBox usernameBox(250, 150, 300, 30, "Username:", font);
    TextBox emailBox(250, 220, 300, 30, "Email:", font);
    TextBox passwordBox(250, 290, 300, 30, "Password:", font);
    TextBox confirmPasswordBox(250, 360, 300, 30, "Confirm Password:", font);
    
    // Create register button
    sf::RectangleShape registerButton(sf::Vector2f(150, 40));
    registerButton.setPosition(325, 450);
    registerButton.setFillColor(sf::Color::Blue);
    registerButton.setOutlineThickness(2);
    registerButton.setOutlineColor(sf::Color::White);
    
    sf::Text registerText;
    registerText.setFont(font);
    registerText.setString("Register");
    registerText.setCharacterSize(18);
    registerText.setFillColor(sf::Color::White);
    
    // Center button text
    sf::FloatRect buttonTextBounds = registerText.getLocalBounds();
    registerText.setPosition(
        325 + (150 - buttonTextBounds.width) / 2.0f,
        450 + (40 - buttonTextBounds.height) / 2.0f
    );
    
    // run the program as long as the window is open
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
                
            // Handle text box events
            usernameBox.handleEvent(event);
            emailBox.handleEvent(event);
            passwordBox.handleEvent(event);
            confirmPasswordBox.handleEvent(event);
            
            // Handle register button click
            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                sf::FloatRect buttonBounds = registerButton.getGlobalBounds();
                
                if (buttonBounds.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                    // Registration button clicked
                    std::cout << "Registration attempted!" << std::endl;
                    std::cout << "Username: " << usernameBox.getContent() << std::endl;
                    std::cout << "Email: " << emailBox.getContent() << std::endl;
                    std::cout << "Password: " << passwordBox.getContent() << std::endl;
                    std::cout << "Confirm Password: " << confirmPasswordBox.getContent() << std::endl;
                }
            }
        }
        
        // Update text boxes
        usernameBox.update();
        emailBox.update();
        passwordBox.update();
        confirmPasswordBox.update();
        
        // clear the window with black color
        window.clear(sf::Color::Black);
        
        // Draw everything
        window.draw(titleText);
        usernameBox.draw(window);
        emailBox.draw(window);
        passwordBox.draw(window);
        confirmPasswordBox.draw(window);
        window.draw(registerButton);
        window.draw(registerText);
        
        // end the current frame
        window.display();
    }
    return 0;
}