// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
//
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Game.h"

// Screen dimension constants
constexpr int SCREEN_WIDTH = 960;
constexpr int SCREEN_HEIGHT = 640;

int main(int argc, char** argv)
{
    Game game = Game(SCREEN_WIDTH, SCREEN_HEIGHT);

    if (game.Initialize()) {
        game.RunLoop();
    }

    game.Shutdown();

    return 0;
}