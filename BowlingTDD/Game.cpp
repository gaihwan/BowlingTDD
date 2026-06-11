#include "Game.h"

void Game::roll(int pins) {
    rolls_[current_roll_++] = pins;
}

int Game::score() const {
    int total = 0;
    for (int roll_index = 0; roll_index < 20; ++roll_index)
        total += rolls_[roll_index];
    return total;
}
