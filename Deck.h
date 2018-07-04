#pragma once
#include "Card.h"

#include <algorithm>
#include <array>
#include <random>

namespace Vegas {

template <size_t NumDecks = 1> class Deck {
    static constexpr size_t numCards = 52 * NumDecks;
    std::array<Vegas::Card, numCards> cards;
    size_t nextCardIndex{};

  public:
    constexpr Deck() noexcept {
        uint8_t val{};
        for (auto& card : cards) {
            card = Vegas::Card{val};
            val = (val + 1) % 52;
        }
    }

    void shuffle(std::mt19937& rnd) noexcept {
        std::shuffle(cards.begin(), cards.end(), rnd);
    }

    constexpr Card nextCard() noexcept { return cards[nextCardIndex++]; }

    constexpr size_t size() const noexcept { return numCards - nextCardIndex; }
};
}
