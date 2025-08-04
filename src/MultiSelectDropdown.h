class MultiSelectDropdown {
private:
    sf::RectangleShape mainBox;
    sf::RectangleShape optionBox;
    sf::Text mainText;
    std::vector<sf::Text> options;
    std::vector<sf::RectangleShape> optionRects;
    std::vector<sf::RectangleShape> checkboxes;
    std::vector<bool> selectedItems;
    sf::Font& font;
    bool isOpen;
    std::vector<std::string> items;
    float optionHeight;
    float maxVisibleOptions;
    float scrollOffset;
    sf::RectangleShape scrollBar;
    bool isScrolling;

public:
    MultiSelectDropdown(float x, float y, float width, float height, sf::Font& font,
                       const std::vector<std::string>& items, int maxVisible = 5)
        : font(font), isOpen(false), items(items),
          optionHeight(height), maxVisibleOptions(maxVisible), scrollOffset(0), isScrolling(false)
    {
        selectedItems.resize(items.size(), false);
        
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
        mainText.setString("Select subjects...");

        // Options Container
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

            // Checkbox
            sf::RectangleShape checkbox(sf::Vector2f(16, 16));
            checkbox.setPosition(x + 10, y + height + i * height + (height - 16) / 2);
            checkbox.setFillColor(sf::Color::White);
            checkbox.setOutlineThickness(1);
            checkbox.setOutlineColor(sf::Color(100, 100, 100));
            checkboxes.push_back(checkbox);

            // Option text
            sf::Text optionText;
            optionText.setFont(font);
            optionText.setString(items[i]);
            optionText.setCharacterSize(16);
            optionText.setFillColor(sf::Color::White);
            optionText.setPosition(x + 35, y + height + i * height + 5);
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
                            
                            selectedItems[i] = !selectedItems[i];
                            updateMainText();
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
            checkboxes[i].setPosition(
                optionBox.getPosition().x + 10,
                optionBox.getPosition().y + i * optionHeight + (optionHeight - 16) / 2 - scrollOffset
            );
            options[i].setPosition(
                optionBox.getPosition().x + 35,
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

    void updateMainText() {
        std::vector<std::string> selected;
        for (size_t i = 0; i < selectedItems.size(); ++i) {
            if (selectedItems[i]) {
                selected.push_back(items[i]);
            }
        }
        
        if (selected.empty()) {
            mainText.setString("Select subjects...");
        } else if (selected.size() == 1) {
            mainText.setString(selected[0]);
        } else {
            mainText.setString(selected[0] + " (+" + std::to_string(selected.size() - 1) + " more)");
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
                    
                    // Draw checkbox
                    sf::RectangleShape checkbox = checkboxes[i];
                    if (selectedItems[i]) {
                        checkbox.setFillColor(sf::Color::Blue);
                        // Draw X mark
                        sf::Text checkmark;
                        checkmark.setFont(font);
                        checkmark.setString("X");
                        checkmark.setCharacterSize(12);
                        checkmark.setFillColor(sf::Color::White);
                        checkmark.setStyle(sf::Text::Bold);
                        checkmark.setPosition(checkbox.getPosition().x + 4, checkbox.getPosition().y + 1);
                        window.draw(checkbox);
                        window.draw(checkmark);
                    } else {
                        checkbox.setFillColor(sf::Color::White);
                        window.draw(checkbox);
                    }
                    
                    window.draw(options[i]);
                }
            }
            
            if (items.size() > maxVisibleOptions) {
                window.draw(scrollBar);
            }
        }
    }

    std::vector<std::string> getSelectedItems() const {
        std::vector<std::string> selected;
        for (size_t i = 0; i < selectedItems.size(); ++i) {
            if (selectedItems[i]) {
                selected.push_back(items[i]);
            }
        }
        return selected;
    }

    std::string getSelectedItemsString() const {
        auto selected = getSelectedItems();
        std::string result;
        for (size_t i = 0; i < selected.size(); ++i) {
            if (i > 0) result += ", ";
            result += selected[i];
        }
        return result;
    }

    void setSelectedItems(const std::vector<std::string>& selected) {
        std::fill(selectedItems.begin(), selectedItems.end(), false);
        
        for (const auto& item : selected) {
            auto it = std::find(items.begin(), items.end(), item);
            if (it != items.end()) {
                size_t index = std::distance(items.begin(), it);
                selectedItems[index] = true;
            }
        }
        updateMainText();
    }

    void update() {
        for (size_t i = 0; i < optionRects.size(); ++i) {
            if (selectedItems[i]) {
                optionRects[i].setFillColor(sf::Color(40, 40, 60));
            } else {
                optionRects[i].setFillColor(sf::Color(50, 50, 50));
            }
        }
    }
};
