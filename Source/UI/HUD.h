#pragma once

#include <string>

#include "Elements/UIScreen.h"
#include "../Actors/Fragment.h"

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
    void SetLevelName(const std::string& levelName);
    // TODO: HUD with unlocked skills
    void onFragmentCollected(Fragment::FragmentType type);

private:
    // HUD elements
    // TODO: HUD elements for unlocked skills

    UIText *mTimeText = nullptr; // Text for time
    UIText *mLevelName = nullptr; // Text for level name
    UIText *mScoreCounter = nullptr; // Text for score counter
    UIText *mLivesCounter = nullptr; // Text for lives counter

    UIImage* mDoubleJumpIcon = nullptr;
    UIImage* mDashIcon = nullptr;
    UIImage* mWallJumpIcon = nullptr;
};
