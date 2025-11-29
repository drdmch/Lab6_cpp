#include "game.h"
#include <algorithm>
#include <random>
#include <coroutine>

GuessingCoroutine player_coroutine(int min_val, int max_val) {
    int low = min_val;
    int high = max_val;
    int guess = -1;

    std::random_device rd;
    std::mt19937 gen(rd());

    while (low <= high) {
        if (low == high) {
            guess = low;
        } else {
            std::uniform_int_distribution<int> distrib(low, high);
            guess = distrib(gen);
        }
        co_yield guess;
    }
}

GuessingGame::GuessingGame(int min, int max) : min_val(min), max_val(max) {

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(min_val, max_val);
    secret_number = dist(gen);
    players.emplace_back(std::make_pair("Player A", player_coroutine(min_val, max_val)));
    players.emplace_back(std::make_pair("Player B", player_coroutine(min_val, max_val)));

    std::cout << "Range: [" << min_val << ", " << max_val << "]" << std::endl;
    std::cout << "Secret Number is set." << std::endl;
    std::cout << "Player A and Player B are ready." << std::endl;
}

GuessResult GuessingGame::check_guess(int guess) const {
    if (guess < secret_number) return -1;
    if (guess > secret_number) return 1;
    return 0;
}

void GuessingGame::print_log(const std::string& player_name, int guess, GuessResult result) const {
    std::cout << "Turn: " << player_name << " guesses: " << guess;
    std::cout << " -> Result: ";
    switch (result) {
        case -1:
            std::cout << "Too Low (-1)";
            break;
        case 0:
            std::cout << "Correct (0)! WINNER!";
            break;
        case 1:
            std::cout << "Too High (1)";
            break;
    }
    std::cout << std::endl;
}

void GuessingGame::run_game() {
    std::cout << "\nGAME PROTOCOL\n";

    size_t current_player_index = 0;
    bool game_over = false;


    while (!game_over) {
        std::pair<std::string, GuessingCoroutine>& current_player = players[current_player_index];

        if (current_player.second.done()) {
            current_player_index = (current_player_index + 1) % players.size();
            continue;
        }

        int current_guess = current_player.second.get_guess();
        GuessResult result = check_guess(current_guess);

        print_log(current_player.first, current_guess, result);

        if (result == 0) {
            game_over = true;
            std::cout << "\nGAME RESULT" << std::endl;
            std::cout << "WINNER: " << current_player.first << std::endl;
            std::cout << "The secret number was: " << secret_number << std::endl;
        } else {
            current_player.second.send_result(result);
            current_player_index = (current_player_index + 1) % players.size();
        }

        if (std::all_of(players.begin(), players.end(), [](const auto& p){ return p.second.done(); })) {
            std::cout << "\nGAME RESULT" << std::endl;
            std::cout << "No winner! All players exhausted their guesses." << std::endl;
            game_over = true;
        }
    }
}
