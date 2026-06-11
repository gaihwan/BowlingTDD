#include "Game.h"

void Game::roll(int pins) {
    rolls_[current_roll_++] = pins;
}

int Game::score() const {
    int total = 0;
    int roll_index = 0;
    for (int frame = 0; frame < 10; ++frame) {
        if (isSpare(roll_index))
            total += 10 + rolls_[roll_index + 2];
        else
            total += rolls_[roll_index] + rolls_[roll_index + 1];
        roll_index += 2;
    }
    return total;
}

bool Game::isSpare(int roll_index) const {
    return rolls_[roll_index] + rolls_[roll_index + 1] == 10;
}
