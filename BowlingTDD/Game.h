#pragma once

class Game {
public:
    void roll(int pins);
    int  score() const;
private:
    bool isSpare(int roll_index) const;
    int rolls_[21]{};
    int current_roll_ = 0;
};
