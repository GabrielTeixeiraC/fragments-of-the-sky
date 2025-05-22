// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
//
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Game.h"

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 448;

int main(int argc, char* argv[])
{
    Game game = Game(SCREEN_WIDTH, SCREEN_HEIGHT);
    bool success = game.Initialize();
    if (success)
    {
        game.RunLoop();
    }
    game.Shutdown();
    return 0;
}
