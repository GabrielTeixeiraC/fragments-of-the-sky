#pragma once

#include <string>

#include "Elements/UIImage.h"
#include "Elements/UIScreen.h"
#include "../Actors/Fragment.h"
#include "Elements/UIRotatingImage.h"

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

    // TODO: HUD with unlocked skills
    void onFragmentCollected(Fragment::FragmentType type);
    
    // Method to restore collected fragments state (for level transitions)
    void RestoreFragmentStates(bool hasDoubleJump, bool hasDash, bool hasWallJump);
    
    // Method to reset all fragment states (for death)
    void ResetFragmentStates();
    
    // Method to remove a specific fragment from display
    void RemoveFragmentFromDisplay(Fragment::FragmentType type);

    void Update(float deltaTime) override;
    void Draw(SDL_Renderer* renderer) override;
    
    // Compass spin control
    void SetSpinSpeed(float spinSpeed) { mSpinSpeed = spinSpeed; }
    float GetSpinSpeed() const { return mSpinSpeed; }

private:
    // HUD elements
    // TODO: HUD elements for unlocked skills

    UIText *mTimeText = nullptr; // Text for time
    UIText *mScoreCounter = nullptr; // Text for score counter
    UIText *mLivesCounter = nullptr; // Text for lives counter

    UIImage* mAerisIcon = nullptr;
    UIImage* mDoubleJumpIcon = nullptr;
    UIImage* mDashIcon = nullptr;
    UIImage* mWallJumpIcon = nullptr;
    UIRotatingImage* mCompassNeedle;
    
    // Compass spin variables
    float mSpinSpeed; // 0 = normal behavior, > 0 = spinning (up to 1.0)
    float mCurrentSpinRotation; // Accumulated rotation for spinning
    float mSpinDirection; // 1.0 = clockwise, -1.0 = counter-clockwise
};
