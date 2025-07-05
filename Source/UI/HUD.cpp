#include "HUD.h"
#include "../Game.h"
#include "Elements/UIText.h"
#include "../Actors/Aeris.h"
#include "../Actors/Fragment.h"
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

    // TODO 3.: Adicione um texto com a string "World" à esquerda do texto "Time", como no jogo original.
    mLevelName = AddText("World", Vector2(WORD_OFFSET, HUD_POS_Y),
                         Vector2(CHAR_WIDTH * 3, WORD_HEIGHT), POINT_SIZE);


    // TODO 4.: Adicione um texto com a string "1-1" logo abaixo do texto "World".
    mLevelName = AddText("1-1", Vector2(WORD_OFFSET, HUD_POS_Y + WORD_HEIGHT),
                         Vector2(CHAR_WIDTH * 3, WORD_HEIGHT), POINT_SIZE);

    float iconSize = 32.0f;
    float iconSpacing = 8.0f;
    float iconStartX = WORD_OFFSET;
    float iconY = HUD_POS_Y + WORD_HEIGHT * 4;

    mDoubleJumpIcon = AddImage("../Assets/Sprites/Blocks/BlockA.png",
                               Vector2(iconStartX, iconY),
                               Vector2(iconSize, iconSize));
    mDoubleJumpIcon->SetVisible(false);

    mDashIcon = AddImage("../Assets/Sprites/Blocks/BlockB.png",
                         Vector2(iconStartX + iconSize + iconSpacing, iconY),
                         Vector2(iconSize, iconSize));
    mDashIcon->SetVisible(false);

    mWallJumpIcon = AddImage("../Assets/Sprites/Blocks/BlockC.png",
                             Vector2(iconStartX + 2 * (iconSize + iconSpacing), iconY),
                             Vector2(iconSize, iconSize));
    mWallJumpIcon->SetVisible(false);

    // Create HUD elements here
    // Compass Frame
    mImages.push_back(new UIImage(game->GetRenderer(), "../Assets/UI/compass.png", Vector2(576, 10), Vector2(128, 128)));
    
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
            
            // Find all nearby fragments
            std::vector<class Actor*> nearbyActors = mGame->GetNearbyActors(playerPos, 50); // Search in a large radius
            
            Fragment* closestFragment = nullptr;
            float closestDistance = std::numeric_limits<float>::max();
            
            for (Actor* actor : nearbyActors) {
                Fragment* fragment = dynamic_cast<Fragment*>(actor);
                if (fragment) {
                    float distance = (fragment->GetPosition() - playerPos).Length();
                    if (distance < closestDistance) {
                        closestDistance = distance;
                        closestFragment = fragment;
                    }
                }
            }
            
            if (closestFragment) {
                Vector2 targetPos = closestFragment->GetPosition();
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