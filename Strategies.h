#pragma once
#include "BlackJack.h"

namespace BlackJack {
struct HumanInput : public Strategy {
    size_t chooseBet(const CardCount& count) override;
    Action chooseAction(const PlayerState::Stack& stack, Vegas::Card dealerCard,
                        const CardCount& count) override;
};

struct ByTheBook : public Strategy {
    Action chooseAction(const PlayerState::Stack& stack, Vegas::Card dealerCard,
                        const CardCount&) final override;
};

struct ByTheBookFlatBet : public ByTheBook {
    size_t chooseBet(const CardCount&) final override { return 10; }
};
}
