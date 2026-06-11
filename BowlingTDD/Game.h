#pragma once

class Game {
public:
    void roll(int pins) noexcept;
    int  score() const noexcept;
private:
    bool isStrike(int roll_index) const noexcept;
    bool isSpare(int roll_index) const noexcept;
    static constexpr int kTotalFrames = 10;
    int rolls_[21]{};  // 10 frames x 2 rolls + 1 bonus = 21 max
    int current_roll_ = 0;
};
