#include <gtest/gtest.h>
#include "Game.h"

namespace {

void rollMany(Game& game, int n, int pins) {
    for (int i = 0; i < n; ++i)
        game.roll(pins);
}

void rollSpare(Game& game) {
    game.roll(5);
    game.roll(5);
}

void rollStrike(Game& game) {
    game.roll(10);
}

}  // namespace

TEST(BowlingGameTest, GutterGameScoresZero) {
    // Arrange
    Game game;

    // Act
    rollMany(game, 20, 0);

    // Assert
    ASSERT_EQ(0, game.score());
}

TEST(BowlingGameTest, AllOnesScoresTwenty) {
    // Arrange
    Game game;

    // Act
    rollMany(game, 20, 1);

    // Assert
    ASSERT_EQ(20, game.score());
}
