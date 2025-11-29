#include "game.h"
#include <iostream>
#include <stdexcept>

int main() {
    std::cout << "NUMBER GUESSING GAME" << std::endl;

    try {
        GuessingGame game(1, 100);
        game.run_game();
    } catch (const std::exception& e) {
        std::cerr << "An error occurred: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "An unknown error occurred." << std::endl;
        return 1;
    }

    return 0;
}