#pragma once

class Game {
public:
    void roll(int pins);
    int  score() const;
private:
    int rolls_[21]{};
    int current_roll_ = 0;
};
