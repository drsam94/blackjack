#include "Strategies.h"

#include <iostream>

namespace BlackJack {
size_t HumanInput::chooseBet(const CardCount&) {
    size_t ret;
    std::cout << "Choose bet" << std::endl;
    std::cin >> ret;
    return ret;
}

Action HumanInput::chooseAction(const PlayerState::Stack& stack,
                                Vegas::Card dealerCard,
                                const CardCount& count) {
    std::cout << "cards: " << stack.toString() << "\n";
    std::cout << "dealer: " << dealerCard.toString() << "\n";
    std::cout << "count: " << count.toString() << std::endl;
    std::string choice;
    std::cin >> choice;
    if (choice[0] == 'h') {
        return Action::Hit;
    }
    if (choice[0] == 's') {
        return Action::Stay;
    }
    if (choice[0] == '2') {
        return Action::Split;
    }
    if (choice[0] == 'd') {
        return Action::DoubleDown;
    }
    return Action::Stay;
}

// This is a little simplified
Action ByTheBook::chooseAction(const PlayerState::Stack& stack,
                               Vegas::Card dealerCard, const CardCount&) {
    bool isSoft;
    const auto sum = bjSum(stack.cards, &isSoft);
    const bool isBustCard = static_cast<uint8_t>(dealerCard.value()) <
                            static_cast<uint8_t>(Vegas::CardValue::_7);
    const bool isFaceCard = static_cast<uint8_t>(dealerCard.value()) >
                            static_cast<uint8_t>(Vegas::CardValue::_9);
    if (stack.cards.size() == 2) {
        // First choice
        if (const auto firstValue = stack.cards.front().value();
            firstValue == stack.cards.back().value()) {
            if (sum >= 18) {
                return Action::Stay;
            }
            if (firstValue == Vegas::CardValue::Ace ||
                firstValue == Vegas::CardValue::_8) {
                return Action::Split;
            }
            if (isBustCard && firstValue != Vegas::CardValue::_5) {
                return Action::Split;
            }
        }
        if (sum == 11) {
            return Action::DoubleDown;
        }
        if (sum == 10 && !isFaceCard) {
            return Action::DoubleDown;
        }
        if (isBustCard && isSoft) {
            return Action::DoubleDown;
        }
    }
    if (sum <= 11) {
        return Action::Hit;
    }
    if (sum > 17) {
        return Action::Stay;
    }
    if (isBustCard) {
        return Action::Stay;
    }
    return Action::Hit;
}
}
