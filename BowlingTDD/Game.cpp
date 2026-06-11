#include "Game.h"

void Game::roll(int pins) noexcept {
    rolls_[current_roll_++] = pins;
}

int Game::score() const noexcept {
    int total = 0;
    int roll_index = 0;
    for (int frame = 0; frame < 10; ++frame) {
        if (isStrike(roll_index)) {
            total += 10 + rolls_[roll_index + 1] + rolls_[roll_index + 2];
            roll_index += 1;
        } else if (isSpare(roll_index)) {
            total += 10 + rolls_[roll_index + 2];
            roll_index += 2;
        } else {
            total += rolls_[roll_index] + rolls_[roll_index + 1];
            roll_index += 2;
        }
    }
    return total;
}

bool Game::isStrike(int roll_index) const noexcept {
    return rolls_[roll_index] == 10;
}

bool Game::isSpare(int roll_index) const noexcept {
    return rolls_[roll_index] + rolls_[roll_index + 1] == 10;
}
