#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <coroutine>
#include <utility>

using GuessResult = int;

struct GuessingCoroutine {
    struct promise_type {
        int current_guess;

        GuessingCoroutine get_return_object() {
            return GuessingCoroutine{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        std::suspend_always initial_suspend() noexcept { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void unhandled_exception() {}
        void return_void() {}

        std::suspend_always yield_value(int guess) {
            current_guess = guess;
            return {};
        }
    };

    std::coroutine_handle<promise_type> handle;

    GuessingCoroutine(std::coroutine_handle<promise_type> h) : handle(h) {}
    GuessingCoroutine(GuessingCoroutine&& other) noexcept : handle(other.handle) { other.handle = nullptr; }
    GuessingCoroutine& operator=(GuessingCoroutine&& other) noexcept {
        if (this != &other) {
            if (handle) handle.destroy();
            handle = other.handle;
            other.handle = nullptr;
        }
        return *this;
    }
    GuessingCoroutine(const GuessingCoroutine&) = delete;
    GuessingCoroutine& operator=(const GuessingCoroutine&) = delete;

    ~GuessingCoroutine() { if (handle) handle.destroy(); }

    bool done() const { return handle.done(); }
    int get_guess() const { return handle.promise().current_guess; }
    void resume() { handle.resume(); }

    void send_result(GuessResult result) {
        resume();
    }
};

GuessingCoroutine player_a_coroutine(int min_val, int max_val);

GuessingCoroutine player_b_coroutine(int min_val, int max_val);

class GuessingGame {
public:
    GuessingGame(int min, int max);
    void run_game();

private:
    int min_val;
    int max_val;
    int secret_number;
    std::vector<std::pair<std::string, GuessingCoroutine>> players;

    GuessResult check_guess(int guess) const;
    void print_log(const std::string& player_name, int guess, GuessResult result) const;
};