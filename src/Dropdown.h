class Dropdown {
private:
    sf::RectangleShape mainBox;
    sf::RectangleShape optionBox;
    sf::Text mainText;
    std::vector<sf::Text> options;
    std::vector<sf::RectangleShape> optionRects;
    sf::Font& font;
    bool isOpen;
    unsigned int selectedIndex;
    std::vector<std::string> items;
    float optionHeight;
    float maxVisibleOptions;
    float scrollOffset;
    sf::RectangleShape scrollBar;
    bool isScrolling;

public:
    Dropdown(float x, float y, float width, float height, sf::Font& font,
                const std::vector<std::string>& items, int maxVisible = 3)
        : font(font), isOpen(false), selectedIndex(0), items(items),
          optionHeight(height), maxVisibleOptions(maxVisible), scrollOffset(0), isScrolling(false)
    {
        // Main Box - match TextBox color scheme
        mainBox.setSize(sf::Vector2f(width, height));
        mainBox.setPosition(x, y);
        mainBox.setFillColor(sf::Color(50, 50, 50));
        mainBox.setOutlineThickness(2);
        mainBox.setOutlineColor(sf::Color::White);

        // Main Text
        mainText.setFont(font);
        mainText.setCharacterSize(16);
        mainText.setFillColor(sf::Color::White);
        mainText.setPosition(x + 5, y + 5);
        if (!items.empty()) {
            mainText.setString(items[0]);
        }

        // Options Container - same width as main box
        optionBox.setSize(sf::Vector2f(width, 0));
        optionBox.setPosition(x, y + height);
        optionBox.setFillColor(sf::Color(50, 50, 50));
        optionBox.setOutlineThickness(2);
        optionBox.setOutlineColor(sf::Color::White);

        // Create options
        for (size_t i = 0; i < items.size(); ++i) {
            // Option background
            sf::RectangleShape optionRect(sf::Vector2f(width - 4, height));
            optionRect.setPosition(x + 2, y + height + i * height);
            optionRect.setFillColor(sf::Color(50, 50, 50));
            optionRect.setOutlineThickness(0);
            optionRects.push_back(optionRect);

            // Option text
            sf::Text optionText;
            optionText.setFont(font);
            optionText.setString(items[i]);
            optionText.setCharacterSize(16);
            optionText.setFillColor(sf::Color::White);
            optionText.setPosition(x + 5, y + height + i * height + 5);
            options.push_back(optionText);
        }

        // Initialize scroll bar
        scrollBar.setFillColor(sf::Color(150, 150, 150));
        scrollBar.setOutlineThickness(0);
    }

    void handleEvent(const sf::Event& event, const sf::RenderWindow& window) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        
        if (event.type == sf::Event::MouseButtonPressed) {
            if (mainBox.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                isOpen = !isOpen;
                optionBox.setSize(sf::Vector2f(
                    mainBox.getSize().x,
                    isOpen ? std::min(optionHeight * maxVisibleOptions, 
                                    optionHeight * items.size()) : 0
                ));
                updateScrollBar();
            }
            else if (isOpen) {
                sf::FloatRect dropdownArea = optionBox.getGlobalBounds();
                if (!dropdownArea.contains(mousePos.x, mousePos.y)) {
                    isOpen = false;
                    optionBox.setSize(sf::Vector2f(mainBox.getSize().x, 0));
                } else {
                    for (size_t i = 0; i < optionRects.size(); ++i) {
                        sf::FloatRect optionBounds = optionRects[i].getGlobalBounds();
                        
                        if (optionBounds.top >= optionBox.getPosition().y && 
                            optionBounds.top < optionBox.getPosition().y + optionBox.getSize().y &&
                            optionBounds.contains(mousePos.x, mousePos.y)) {
                            selectedIndex = i;
                            mainText.setString(items[i]);
                            isOpen = false;
                            optionBox.setSize(sf::Vector2f(mainBox.getSize().x, 0));
                            break;
                        }
                    }
                    
                    if (items.size() > maxVisibleOptions && 
                        scrollBar.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        isScrolling = true;
                    }
                }
            }
        }
        else if (event.type == sf::Event::MouseButtonReleased) {
            isScrolling = false;
        }
        else if (event.type == sf::Event::MouseMoved && isScrolling) {
            float relativeY = mousePos.y - optionBox.getPosition().y;
            float scrollAreaHeight = optionBox.getSize().y;
            float scrollRatio = std::max(0.f, std::min(1.f, relativeY / scrollAreaHeight));
            
            float maxScrollOffset = std::max(0.f, (items.size() - maxVisibleOptions) * optionHeight);
            scrollOffset = scrollRatio * maxScrollOffset;
            
            updateOptionsPosition();
            updateScrollBar();
        }
        else if (event.type == sf::Event::MouseWheelScrolled && isOpen) {
            if (optionBox.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                float maxScrollOffset = std::max(0.f, (items.size() - maxVisibleOptions) * optionHeight);
                scrollOffset = std::max(0.f, std::min(
                    scrollOffset - event.mouseWheelScroll.delta * optionHeight, 
                    maxScrollOffset
                ));
                updateOptionsPosition();
                updateScrollBar();
            }
        }
    }

    void updateOptionsPosition() {
        for (size_t i = 0; i < optionRects.size(); ++i) {
            optionRects[i].setPosition(
                optionBox.getPosition().x + 2, 
                optionBox.getPosition().y + i * optionHeight - scrollOffset
            );
            options[i].setPosition(
                optionBox.getPosition().x + 5,
                optionBox.getPosition().y + i * optionHeight + 5 - scrollOffset
            );
        }
    }

    void updateScrollBar() {
        if (items.size() > maxVisibleOptions && isOpen) {
            float scrollHeight = (maxVisibleOptions / static_cast<float>(items.size())) * 
                               optionBox.getSize().y;
            float maxScrollOffset = std::max(0.f, (items.size() - maxVisibleOptions) * optionHeight);
            float scrollPos = maxScrollOffset > 0 ? 
                            (scrollOffset / maxScrollOffset) * (optionBox.getSize().y - scrollHeight) : 0;
            
            scrollBar.setSize(sf::Vector2f(8, scrollHeight));
            scrollBar.setPosition(
                optionBox.getPosition().x + optionBox.getSize().x - 10, 
                optionBox.getPosition().y + scrollPos
            );
        }
    }

    void draw(sf::RenderWindow& window) {
        window.draw(mainBox);
        window.draw(mainText);
        
        if (isOpen && optionBox.getSize().y > 0) {
            window.draw(optionBox);
            
            for (size_t i = 0; i < optionRects.size(); ++i) {
                if (optionRects[i].getPosition().y + optionHeight >= optionBox.getPosition().y && 
                    optionRects[i].getPosition().y <= optionBox.getPosition().y + optionBox.getSize().y) {
                    window.draw(optionRects[i]);
                    window.draw(options[i]);
                }
            }
            
            if (items.size() > maxVisibleOptions) {
                window.draw(scrollBar);
            }
        }
    }

    std::string getSelectedItem() const {
        if (selectedIndex < items.size()) {
            return items[selectedIndex];
        }
        return "";
    }

    void update() {
        for (size_t i = 0; i < optionRects.size(); ++i) {
            if (i == selectedIndex) {
                optionRects[i].setFillColor(sf::Color::Blue);
            } else {
                optionRects[i].setFillColor(sf::Color(50, 50, 50));
            }
        }
    }
};
