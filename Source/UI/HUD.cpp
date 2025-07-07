#include "HUD.h"
#include "../Game.h"
#include "Elements/UIText.h"
#include "../Actors/Aeris.h"
#include "../Actors/Fragment.h"
#include "../Actors/FlagBlock.h"
#include "../Utils/Random.h"
#include <cmath>
#include <limits>

HUD::HUD(class Game* game, const std::string& fontName, UIType uiType)
    : UIScreen(game, fontName, uiType)
    , mCompassNeedle(nullptr)
    , mSpinSpeed(0.0f)
    , mCurrentSpinRotation(0.0f)
    , mSpinDirection(1.0f)
{

    mAerisIcon = AddImage("../Assets/Sprites/Aeris/hudAeris.png",
                          Vector2(WORD_OFFSET, HUD_POS_Y),
                          Vector2(64, 64));

    mAerisIcon->SetVisible(true);

    int textPosY = HUD_POS_Y + WORD_HEIGHT * 2;
    int textOffsetX = WORD_OFFSET + CHAR_WIDTH * 12;
    int rightMargin = 32;
    int screenWidth = 0, screenHeight = 0;

    SDL_Renderer* sdlRenderer = mGame->GetRenderer();
    SDL_GetRendererOutputSize(sdlRenderer, &screenWidth, &screenHeight);
    int levelTextX = screenWidth - rightMargin - CHAR_WIDTH * 5;

    mLevelName = AddText("Level", Vector2(levelTextX, HUD_POS_Y),
                         Vector2(CHAR_WIDTH * 3, WORD_HEIGHT * 2), POINT_SIZE * 1.5f);

    // Add "1-1" text just below "World"
    mLevelName = AddText("1", Vector2(levelTextX + CHAR_WIDTH * 5, HUD_POS_Y),
                         Vector2(CHAR_WIDTH * 3, WORD_HEIGHT * 2), POINT_SIZE * 1.5f);

    float iconSize = 32.0f;
    float iconSpacing = 8.0f;
    // Place icons to the right of Aeris icon
    float iconStartX = WORD_OFFSET + 64 + iconSpacing; // 64 is Aeris icon width
    float iconY = HUD_POS_Y + (64 - iconSize) / 2.0f;  // Vertically center with Aeris icon

    mDoubleJumpIcon = AddImage("../Assets/Sprites/Fragments/doubleJump.png",
                               Vector2(iconStartX, iconY),
                               Vector2(iconSize, iconSize));
    mDoubleJumpIcon->SetVisible(false);

    mDashIcon = AddImage("../Assets/Sprites/Fragments/dash.png",
                         Vector2(iconStartX + iconSize + iconSpacing, iconY),
                         Vector2(iconSize, iconSize));
    mDashIcon->SetVisible(false);

    mWallJumpIcon = AddImage("../Assets/Sprites/Fragments/wall.png",
                             Vector2(iconStartX + 2 * (iconSize + iconSpacing), iconY),
                             Vector2(iconSize, iconSize));
    mWallJumpIcon->SetVisible(false);

    // Create HUD elements here
    // Compass Frame
    mImages.push_back(new UIImage(game->GetRenderer(), "../Assets/UI/compass.png", Vector2(576, 10), Vector2(128, 128)));
    mImages.push_back(new UIImage(game->GetRenderer(), "../Assets/Sprites/Aeris/AerisIdle.png", Vector2(WORD_OFFSET, HUD_POS_Y), Vector2(CHAR_WIDTH * 3, WORD_HEIGHT)));
    // Compass Needle (rotating)
    mCompassNeedle = new UIRotatingImage(game->GetRenderer(), "../Assets/UI/pointer.png", Vector2(576, 10), Vector2(128, 128));
}

HUD::~HUD()
{
    for (UIImage* image : mImages) {
        delete image;
    }
    mImages.clear();

    if (mCompassNeedle) {
        delete mCompassNeedle;
        mCompassNeedle = nullptr;
    }
}

void HUD::SetLevelName(const std::string &levelName)
{
    mLevelName->SetText(levelName);
}

void HUD::onFragmentCollected(Fragment::FragmentType type)
{
    if (type == Fragment::FragmentType::DoubleJump && mDoubleJumpIcon) {
        mDoubleJumpIcon->SetVisible(true);
    }
    else if (type == Fragment::FragmentType::Dash && mDashIcon) {
        mDashIcon->SetVisible(true);
    }
    else if (type == Fragment::FragmentType::WallJump && mWallJumpIcon) {
        mWallJumpIcon->SetVisible(true);
    }
}

void HUD::RestoreFragmentStates(bool hasDoubleJump, bool hasDash, bool hasWallJump)
{
    if (hasDoubleJump && mDoubleJumpIcon) {
        mDoubleJumpIcon->SetVisible(true);
    }
    if (hasDash && mDashIcon) {
        mDashIcon->SetVisible(true);
    }
    if (hasWallJump && mWallJumpIcon) {
        mWallJumpIcon->SetVisible(true);
    }
}

void HUD::ResetFragmentStates()
{
    if (mDoubleJumpIcon) {
        mDoubleJumpIcon->SetVisible(false);
    }
    if (mDashIcon) {
        mDashIcon->SetVisible(false);
    }
    if (mWallJumpIcon) {
        mWallJumpIcon->SetVisible(false);
    }
}

void HUD::RemoveFragmentFromDisplay(Fragment::FragmentType type)
{
    switch (type) {
        case Fragment::FragmentType::DoubleJump:
            if (mDoubleJumpIcon) {
                mDoubleJumpIcon->SetVisible(false);
            }
            break;
        case Fragment::FragmentType::Dash:
            if (mDashIcon) {
                mDashIcon->SetVisible(false);
            }
            break;
        case Fragment::FragmentType::WallJump:
            if (mWallJumpIcon) {
                mWallJumpIcon->SetVisible(false);
            }
            break;
    }
}

void HUD::Update(float deltaTime)
{
    UIScreen::Update(deltaTime);

    // Handle compass spinning behavior
    if (mSpinSpeed > 0.0f) {
        // Check for direction flip chance (mSpinSpeed % chance per frame)
        float flipChance = mSpinSpeed * 0.01f; // 1% at max spin speed
        if (Random::GetFloatRange(0.0f, 1.0f) < flipChance) {
            mSpinDirection *= -1.0f; // Flip direction
        }

        // Spin the compass randomly
        const float SPIN_RATE_MULTIPLIER = 8.0f; // Base spin rate (radians per second at mSpinSpeed = 1.0)

        // Add some randomness to the spin rate
        float randomFactor = 0.5f + Random::GetFloatRange(0.0f, 1.0f); // 0.5 to 1.5
        float spinRate = SPIN_RATE_MULTIPLIER * mSpinSpeed * randomFactor * mSpinDirection;

        // Accumulate rotation
        mCurrentSpinRotation += spinRate * deltaTime;

        // Keep rotation in reasonable range
        while (mCurrentSpinRotation > 2.0f * M_PI) {
            mCurrentSpinRotation -= 2.0f * M_PI;
        }

        mCompassNeedle->SetRotation(mCurrentSpinRotation);
    }
    else {
        // Normal behavior: point to closest fragment
        const class Aeris* aeris = mGame->GetAeris();

        if (aeris) {
            Vector2 playerPos = aeris->GetPosition();

                    // Find all nearby fragments and flag blocks
        std::vector<class Actor*> nearbyActors = mGame->GetNearbyActors(playerPos, 50); // Search in a large radius

        Fragment* closestFragment = nullptr;
        FlagBlock* closestFlagBlock = nullptr;
        float closestFragmentDistance = std::numeric_limits<float>::max();
        float closestFlagBlockDistance = std::numeric_limits<float>::max();

        for (Actor* actor : nearbyActors) {
            Fragment* fragment = dynamic_cast<Fragment*>(actor);
            if (fragment) {
                float distance = (fragment->GetPosition() - playerPos).Length();
                if (distance < closestFragmentDistance) {
                    closestFragmentDistance = distance;
                    closestFragment = fragment;
                }
            }
            
            FlagBlock* flagBlock = dynamic_cast<FlagBlock*>(actor);
            if (flagBlock) {
                float distance = (flagBlock->GetPosition() - playerPos).Length();
                if (distance < closestFlagBlockDistance) {
                    closestFlagBlockDistance = distance;
                    closestFlagBlock = flagBlock;
                }
            }
        }

        // Point to fragment if available, otherwise point to flag block
        Actor* targetActor = nullptr;
        if (closestFragment) {
            targetActor = closestFragment;
        } else if (closestFlagBlock) {
            targetActor = closestFlagBlock;
        }

        if (targetActor) {
            Vector2 targetPos = targetActor->GetPosition();
            Vector2 direction = targetPos - playerPos;
            float angle = atan2(direction.y, direction.x);

            // Adjust angle since pointer image points up by default
            // In screen coordinates, "up" is negative y, so we add π/2
            angle += M_PI / 2.0f;

            mCompassNeedle->SetRotation(angle);
        }
        }
    }
}

void HUD::Draw(SDL_Renderer* renderer)
{
    // Draw base UI elements
    UIScreen::Draw(renderer);

    // Draw compass needle
    if (mCompassNeedle) {
        mCompassNeedle->Draw(renderer, mPos);
    }
}