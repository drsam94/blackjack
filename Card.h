#pragma once
#include <cstdint>
#include <string>

namespace Vegas {

enum class CardValue : uint8_t {
    _2 = 0,
    _3,
    _4,
    _5,
    _6,
    _7,
    _8,
    _9,
    _10,
    Jack = 9,
    Queen = 10,
    King = 11,
    Ace = 12
};

enum class Suit : uint8_t { Heart, Diamond, Spade, Club };

class Card {
    uint8_t val{};

  public:
    constexpr Card() = default;
    explicit constexpr Card(uint8_t v) noexcept : val{v} {}
    constexpr CardValue value() const noexcept {
        return static_cast<CardValue>(val % 13);
    }

    constexpr Suit suit() const noexcept { return static_cast<Suit>(val / 13); }

    // TODO: use stream operators or something else smart instead?
    // Perf isn't much of an issue, will mostly be used for debugging
    std::string toString() const noexcept {
        std::string s;
        switch (auto v = value(); v) {
        case CardValue::Jack:
            s += 'J';
            break;
        case CardValue::Queen:
            s += 'Q';
            break;
        case CardValue::King:
            s += 'K';
            break;
        case CardValue::Ace:
            s += 'A';
            break;
        default:
            s = std::to_string(static_cast<unsigned int>(v) + 2);
        }
        switch (suit()) {
        case Suit::Heart:
            s += 'H';
            break;
        case Suit::Diamond:
            s += 'D';
            break;
        case Suit::Spade:
            s += 'S';
            break;
        case Suit::Club:
            s += 'C';
            break;
        }
        return s;
    }
};
}
