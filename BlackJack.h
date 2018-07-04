#pragma once
#include "Card.h"
#include "Deck.h"

#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std::string_literals;

namespace BlackJack {
using Deck = Vegas::Deck<4>;

enum class Action : uint8_t { Hit, Stay, Split, DoubleDown };

std::string cardsString(const std::vector<Vegas::Card>& cards) noexcept;

struct PlayerState {
    size_t bet;
    struct Stack {
        std::vector<Vegas::Card> cards;
        bool isDoubleDown = false;
        bool isSplitAceLocked = false;

        std::string toString() const noexcept { return cardsString(cards); }
    };
    std::vector<Stack> stacks;
};

class CardCount {
    long count{};
    const Deck& deck;

  public:
    explicit CardCount(const Deck& d) noexcept : deck{d} {}

    void updateCount(Vegas::Card c) noexcept {
        switch (c.value()) {
        case Vegas::CardValue::_2:
        case Vegas::CardValue::_3:
        case Vegas::CardValue::_4:
        case Vegas::CardValue::_5:
        case Vegas::CardValue::_6:
            ++count;
            break;
        case Vegas::CardValue::_7:
        case Vegas::CardValue::_8:
        case Vegas::CardValue::_9:
            break;
        case Vegas::CardValue::_10:
        case Vegas::CardValue::Jack:
        case Vegas::CardValue::Queen:
        case Vegas::CardValue::King:
        case Vegas::CardValue::Ace:
            --count;
            break;
        }
    }

    long getCount() const noexcept { return count; }

    size_t getCardsLeft() const noexcept { return deck.size(); }

    void reset() noexcept { count = 0; }

    std::string toString() const noexcept {
        return std::to_string(count) + " / "s + std::to_string(getCardsLeft());
    }
};

struct Strategy {
    virtual Action chooseAction(const PlayerState::Stack& stack,
                                Vegas::Card delearCard,
                                const CardCount& count) = 0;

    virtual size_t chooseBet(const CardCount& count) = 0;
    virtual ~Strategy() = default;
    // TODO: insurance?
};

class Game;
struct Player {
    PlayerState state;
    std::unique_ptr<Strategy> strategy;

    explicit Player(std::unique_ptr<Strategy> strat)
        : strategy{std::move(strat)} {}
    void playHand(Game& game);
};

class Game {
    std::vector<Vegas::Card> dealer;
    std::vector<Player> players;
    Vegas::Deck<4> deck;
    CardCount count;
    std::mt19937& rng;

    // TODO: insurance? sidebets?
    std::vector<size_t> payout(bool dealerBlackjack) const noexcept;

    void playDealer() noexcept;

  public:
    Game(std::vector<std::unique_ptr<Strategy>> strategies, std::mt19937& rnd);

    std::vector<size_t> doHand();

    Vegas::Card drawPublic() noexcept;

    Vegas::Card dealerCard() const noexcept { return dealer.back(); }

    const CardCount& getCount() const noexcept { return count; }

    std::string toString() const noexcept;
};

int8_t bjSum(const std::vector<Vegas::Card>& cards,
             bool* isSoft = nullptr) noexcept;

struct StrategyError : public std::logic_error {
    using logic_error::logic_error;
};
}
