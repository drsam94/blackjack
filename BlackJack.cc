#include "BlackJack.h"
#include "ScopeGuard.h"

#include <iostream>
namespace BlackJack {

Game::Game(std::vector<std::unique_ptr<Strategy>> strategies, std::mt19937& rnd)
    : count{deck}, rng{rnd} {
    for (auto&& strat : strategies) {
        players.emplace_back(std::move(strat));
    }
    deck.shuffle(rng);
}

std::vector<size_t> Game::doHand() {
    if (deck.size() < 52) {
        deck.shuffle(rng);
        count.reset();
    }
    for (auto& player : players) {
        player.state.stacks.clear();
        player.state.bet = player.strategy->chooseBet(count);
    }
    dealer.clear();
    for (auto& player : players) {
        player.state.stacks.emplace_back().cards.push_back(drawPublic());
    }
    const auto hiddenDealerCard = deck.nextCard();
    dealer.push_back(hiddenDealerCard);
    ScopeGuard guard{[&]() { count.updateCount(hiddenDealerCard); }};
    for (auto& player : players) {
        player.state.stacks.front().cards.push_back(drawPublic());
    }
    dealer.push_back(drawPublic());

    if (bjSum(dealer) == 21) {
        return payout(true);
    }

    for (auto& player : players) {
        player.playHand(*this);
    }

    playDealer();

    return payout(false);
}

Vegas::Card Game::drawPublic() noexcept {
    const auto nextCard = deck.nextCard();
    count.updateCount(nextCard);
    return nextCard;
}

void Game::playDealer() noexcept {
    bool isSoft;
    auto sum = bjSum(dealer, &isSoft);
    while (sum <= 16 || (sum == 17 && isSoft)) {
        dealer.push_back(drawPublic());
        sum = bjSum(dealer, &isSoft);
    }
}

std::vector<size_t> Game::payout(const bool dealerBlackjack) const noexcept {
    const int8_t dealerValue = bjSum(dealer);
    std::vector<size_t> ret;
    for (const auto& player : players) {
        size_t& payout = ret.emplace_back();
        if (player.state.stacks.size() == 1) {
            const auto& stack = player.state.stacks.front();
            if (stack.cards.size() == 2 && bjSum(stack.cards) == 21) {
                // Player blackjack!
                if (!dealerBlackjack) {
                    payout = (player.state.bet * 3) / 2;
                } else {
                    payout = 0;
                }
                continue;
            }
        }
        for (const auto& stack : player.state.stacks) {
            const int8_t playerValue = bjSum(stack.cards);
            if (playerValue > 21 ||
                (dealerValue <= 21 && playerValue < dealerValue)) {
                payout -= (stack.isDoubleDown ? 2 : 1) * player.state.bet;
            } else if (playerValue <= 21 &&
                       (playerValue > dealerValue || dealerValue > 21)) {
                payout += (stack.isDoubleDown ? 2 : 1) * player.state.bet;
            }
        }
    }
    return ret;
}

std::string cardsString(const std::vector<Vegas::Card>& cards) noexcept {
    std::string ret;
    for (const auto c : cards) {
        ret += c.toString();
        ret += ", ";
    }
    ret.pop_back();
    ret.pop_back();
    return ret;
}

std::string Game::toString() const noexcept {
    std::string ret;
    ret += "dealer: "s + cardsString(dealer) + "\n"s;
    for (const auto& player : players) {
        for (const auto& stack : player.state.stacks) {
            ret += stack.toString();
            if (stack.isDoubleDown) {
                ret += " (2x)";
            }
            ret += "\n";
        }
    }
    return ret;
}

int8_t bjSum(const std::vector<Vegas::Card>& cards, bool* isSoft) noexcept {
    int8_t sum{};
    int8_t aces{};
    if (isSoft != nullptr) {
        *isSoft = false;
    }
    for (auto card : cards) {
        switch (auto value = card.value(); value) {
        case Vegas::CardValue::Jack:
        case Vegas::CardValue::Queen:
        case Vegas::CardValue::King:
            sum += 10;
            break;
        case Vegas::CardValue::Ace:
            ++aces;
            break;
        default:
            sum += 2 + static_cast<int8_t>(value);
        }
    }

    for (; aces > 0; --aces) {
        if (sum + 11 + aces - 1 >= 21) {
            if (isSoft != nullptr) {
                *isSoft = true;
            }
            sum += 11;
        } else {
            sum += 1;
        }
    }
    return sum;
}

void Player::playHand(Game& game) {
    for (auto& stack : state.stacks) {
        bool stay = false;
        while (bjSum(stack.cards) < 21 && !stack.isDoubleDown &&
               !stack.isSplitAceLocked && !stay) {

            switch (auto choice = strategy->chooseAction(
                        stack, game.dealerCard(), game.getCount());
                    choice) {
            case Action::Stay:
                stay = true;
                break;
            case Action::Split: {
                if (stack.cards.size() != 2 ||
                    stack.cards.front().value() != stack.cards.back().value()) {
                    throw StrategyError("Illegal Split with cards "s +
                                        stack.cards.front().toString() + " "s +
                                        stack.cards.back().toString());
                }
                auto& newStack = state.stacks.emplace_back();
                newStack.cards.push_back(stack.cards.back());
                stack.cards.pop_back();
                if (stack.cards.front().value() == Vegas::CardValue::Ace) {
                    stack.isSplitAceLocked = newStack.isSplitAceLocked = true;
                    stack.cards.push_back(game.drawPublic());
                    newStack.cards.push_back(game.drawPublic());
                }
                break;
            }
            case Action::DoubleDown:
                if (stack.cards.size() != 2 || stack.isSplitAceLocked) {
                    throw StrategyError("Illegal double down");
                }
                stack.isDoubleDown = true;
                [[fallthrough]];
            case Action::Hit:
                stack.cards.push_back(game.drawPublic());
                break;

            default:
                throw StrategyError("Illegal choice from strategy");
            }
        }
    }
}
}
