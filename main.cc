#include "BlackJack.h"
#include "Strategies.h"

#include <iostream>
#include <random>
#include <unistd.h>

int main(int argc, char** argv) {
    int opt;

    // 'interactive' mode, (i.e human playing blackjack)
    bool interactive = false;
    size_t numTrials = 10;
    bool flat = false;
    while ((opt = getopt(argc, argv, "ifn:")) != -1) {
        switch (opt) {
        case 'i':
            interactive = true;
            break;
        case 'n':
            numTrials = atoi(optarg);
            break;
        case 'f':
            flat = true;
            break;
        }
    }

    std::vector<std::unique_ptr<BlackJack::Strategy>> strats;
    std::unique_ptr<BlackJack::Strategy> strat;
    if (flat) {
        strat = std::make_unique<BlackJack::ByTheBookFlatBet>();
    } else if (interactive) {
        strat = std::make_unique<BlackJack::HumanInput>();
    } else {
        strat = std::make_unique<BlackJack::ByTheBookVariedBet>();
    }
    strats.emplace_back(std::move(strat));
    std::mt19937 rnd;
    BlackJack::Game game{std::move(strats), rnd};

    std::vector<long> results;
    for (size_t n = 0; n < numTrials; ++n) {
        long money = 0;
        for (size_t i = 0; i < 100; ++i) {
            auto payouts = game.doHand();
            money += payouts.front();
            if (interactive) {
                std::cout << "money: " << money << "\n";
                std::cout << "Game: "
                          << "\n"
                          << game.toString() << std::endl;
            }
        }
        results.push_back(money);
    }
    for (auto val : results) {
        std::cout << val << "\n";
    }

    return 0;
}
