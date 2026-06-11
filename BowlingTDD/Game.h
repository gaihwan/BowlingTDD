#pragma once

class Game {
public:
    void roll(int pins) noexcept;
    int  score() const noexcept;
private:
    bool isStrike(int roll_index) const noexcept;
    bool isSpare(int roll_index) const noexcept;
    int rolls_[21]{};
    int current_roll_ = 0;
};
