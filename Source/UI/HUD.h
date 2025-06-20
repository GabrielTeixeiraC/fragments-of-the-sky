#pragma once

#include <string>

#include "Elements/UIScreen.h"

class HUD : public UIScreen {
public:
    // TODO: fix this (when the game font is defined)
    static const int POINT_SIZE = 24;
    static const int WORD_HEIGHT = 20.0f;
    static const int WORD_OFFSET = 25.0f;
    static const int CHAR_WIDTH = 20.0f;
    static const int HUD_POS_Y = 10.0f;

    HUD(class Game *game, const std::string &fontName, UIType uiType);

    ~HUD();

    // Reinsert the HUD elements
    // TODO: HUD with unlocked skills

private:
    // HUD elements
    // TODO: HUD elements for unlocked skills
};
