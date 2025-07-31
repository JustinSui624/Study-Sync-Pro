#include "Application.h"
#include <iostream>

int main() {
    std::cout << "=== Study Group Finder Debug ===" << std::endl;
    std::cout << "Starting main function..." << std::endl;
    
    try {
        std::cout << "Creating Application object..." << std::endl;
        Application app;
        
        std::cout << "Application created successfully!" << std::endl;
        std::cout << "Starting application run loop..." << std::endl;
        
        app.run();
        
        std::cout << "Application run completed." << std::endl;
    }
    catch (const std::exception& e) {
        std::cout << "❌ Exception caught in main: " << e.what() << std::endl;
        std::cout << "Press Enter to exit..." << std::endl;
        std::cin.get(); // Wait for user input before closing
        return -1;
    }
    catch (...) {
        std::cout << "❌ Unknown exception caught in main!" << std::endl;
        std::cout << "Press Enter to exit..." << std::endl;
        std::cin.get(); // Wait for user input before closing
        return -1;
    }
    
    std::cout << "Main function ending normally." << std::endl;
    return 0;
}