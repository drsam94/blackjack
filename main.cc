#include "BlackJack.h"
#include "Strategies.h"

#include <iostream>
#include <random>

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) {
    std::vector<std::unique_ptr<BlackJack::Strategy>> strats;
    strats.emplace_back(std::make_unique<BlackJack::ByTheBookFlatBet>());
    std::mt19937 rnd;
    BlackJack::Game game{std::move(strats), rnd};

    long money = 0;
    for (size_t i = 0; i < 10; ++i) {
        auto payouts = game.doHand();
        money += payouts.front();
        std::cout << "money: " << money << "\n";
        std::cout << "Game: "
                  << "\n"
                  << game.toString() << std::endl;
    }
    return 0;
}
