#include "Aeris.h"
#include "Block.h"
#include "Fragment.h"
#include "../Game.h"
#include "../UI/HUD.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"

Aeris::Aeris(Game* game, const float forwardSpeed, const float jumpSpeed,
             const float dashSpeed, bool canFallThroughPlatform,
             bool fallingThroughAPlatform, float fallTime)
    : Actor(game)
      , mIsRunning(false)
      , mIsWallCrawling(false)
      , mLifeCount(3)
      , mReceivedDamageRecently(false)
      , mCanReceiveDamageTime(0.0f)
      , mHasUnlockedDoubleJump(false)
      , mHasUnlockedDash(false)
      , mHasUnlockedWallJump(false)
      , mIsDying(false)
      , mForwardSpeed(forwardSpeed)
      , mJumpSpeed(jumpSpeed)
      , mJumpCount(0)
      , mQueuedJumpTime(0.0f)
      , mHasQueuedJump(false)
      , mDashSpeed(dashSpeed)
      , mIsDashing(false)
      , mDashTime(0.0f)
      , mCanFallThroughPlatform(canFallThroughPlatform)
      , mIsFallingThroughPlatform(fallingThroughAPlatform)
      , mFallTime(fallTime)
{
    SetActorType(ActorType::Player);

    mRigidBodyComponent = new RigidBodyComponent(this, 1.0f, 14.0f);
    mColliderComponent = new AABBColliderComponent(
        this, 0, 0, Game::TILE_SIZE - 4.0f, Game::TILE_SIZE,
        ColliderLayer::Player);

    mDrawComponent = new DrawAnimatedComponent(this,
                                               "../Assets/Sprites/Aeris/Aeris.png",
                                               "../Assets/Sprites/Aeris/Aeris.json");

    mDrawComponent->AddAnimation("Dead", {9});
    mDrawComponent->AddAnimation("idle", {2});
    mDrawComponent->AddAnimation("jump", {5});
    mDrawComponent->AddAnimation("run", {6, 7, 8});
    mDrawComponent->AddAnimation("win", {7});
    mDrawComponent->AddAnimation("crawl", {0});
    mDrawComponent->AddAnimation("dash", {1});

    mDrawComponent->SetAnimation("idle");
    mDrawComponent->SetAnimFPS(7.0f);
}

void Aeris::TakeDamage()
{
    if (mReceivedDamageRecently) return;

    if (mLifeCount > 0) {
        mLifeCount--;
        mReceivedDamageRecently = true;
        mCanReceiveDamageTime = TIME_UNTIL_NEXT_DAMAGE;
        return;
    }

    Kill();
}


Vector2 Aeris::Orientation()
{
    if (Math::NearZero(mRotation)) {
        return Vector2::UnitX;
    }
    return Vector2::NegUnitX;
}


void Aeris::SetOnGround()
{
    mJumpCount = 0;
    if (mHasQueuedJump && mQueuedJumpTime > 0.0f) {
        mJumpCount = 0;
        Jump();
    } else {
        mIsOnGround = true;
    }
    mHasQueuedJump = false;
    mQueuedJumpTime = 0.0f;
}


void Aeris::Jump()
{
    mRigidBodyComponent->SetVelocity(
        Vector2(mRigidBodyComponent->GetVelocity().x, mJumpSpeed));
    mIsOnGround = false;
    mJumpCount++;
    mGame->GetAudio()->PlaySound("jump.wav");
}

void Aeris::OnProcessInput(const uint8_t* state)
{
    if (mGame->GetGamePlayState() != Game::GamePlayState::Playing) return;
    if (mIsFallingThroughPlatform) return;

    if (state[SDL_SCANCODE_D]) {
        mRigidBodyComponent->ApplyForce(Vector2::UnitX * mForwardSpeed);
        mRotation = 0.0f;
        mIsRunning = true;
    }

    if (state[SDL_SCANCODE_A]) {
        mRigidBodyComponent->ApplyForce(Vector2::UnitX * -mForwardSpeed);
        mRotation = Math::Pi;
        mIsRunning = true;
    }

    if (!state[SDL_SCANCODE_D] && !state[SDL_SCANCODE_A]) {
        mIsRunning = false;
        mIsWallCrawling = false;
    }
}

void Aeris::OnHandleKeyPress(const int key, const bool isPressed)
{
    if (mGame->GetGamePlayState() != Game::GamePlayState::Playing) return;

    // Jump (now allows jumping from wall)
    if (key == SDLK_w && isPressed && !mIsFallingThroughPlatform) {
        if (mJumpCount < MAX_JUMP_COUNT) {
            if (mJumpCount == 0) {
                // Check if we're wall jumping
                if (mIsWallCrawling && mHasUnlockedWallJump) {
                    // Wall jump: move away from wall and set velocity directly
                    Vector2 wallJumpVelocity;
                    Vector2 positionOffset;
                    
                    // Push away from the wall based on player's facing direction
                    if (mRotation == 0.0f) {
                        // Facing right, jump left
                        wallJumpVelocity = Vector2(-600.0f, mJumpSpeed * 0.8f);
                        positionOffset = Vector2(-2.0f, 0);  // Move slightly left
                    } else {
                        // Facing left, jump right
                        wallJumpVelocity = Vector2(600.0f, mJumpSpeed * 0.8f);
                        positionOffset = Vector2(2.0f, 0);   // Move slightly right
                    }
                    
                    // Move player slightly away from wall to avoid immediate collision
                    mPosition += positionOffset;
                    
                    // Set velocity directly with both horizontal and vertical components
                    mRigidBodyComponent->SetVelocity(wallJumpVelocity);
                    mIsWallCrawling = false; // Exit wall crawling state
                    mIsOnGround = false;
                    mJumpCount++;
                    mGame->GetAudio()->PlaySound("jump.wav");
                } else {
                    // Normal jump
                    Jump();
                }
            } else {
                if (mHasUnlockedDoubleJump) {
                    Jump();
                }
            }
        } else {
            mHasQueuedJump = true;
            mQueuedJumpTime = QUEUED_JUMP_TIME;
        }
    }

    // Dash
    if (key == SDLK_LSHIFT && isPressed && mHasUnlockedDash && !mIsDashing && !
        mIsFallingThroughPlatform) {
        mIsDashing = true;
        mDashTime = DASH_TIME;
        mRigidBodyComponent->ApplyForce(Orientation() * mDashSpeed);

        // Play dash sound
        mGame->GetAudio()->PlaySound("dash.wav");
    }

    // Platform fall
    if (key == SDLK_s && mCanFallThroughPlatform) {
        SDL_Log("perform platform fall");
        mIsFallingThroughPlatform = true;
        mFallTime = FALLTHROUGH_TIMER;
        mColliderComponent->SetEnabled(false);
    }
}

void Aeris::OnUpdate(float deltaTime)
{
    // Check if Aeris is off the ground
    if (mRigidBodyComponent && mRigidBodyComponent->GetVelocity().y != 0) {
        mIsOnGround = false;
        mCanFallThroughPlatform = false;
    }

    if (mReceivedDamageRecently) {
        mCanReceiveDamageTime -= deltaTime;
        if (mCanReceiveDamageTime <= 0.0f) {
            mReceivedDamageRecently = false;
        }
    }

    if (mHasQueuedJump) {
        mQueuedJumpTime -= deltaTime;
        if (mQueuedJumpTime <= 0.0f) {
            mHasQueuedJump = false;
        }
    }

    if (mIsDashing) {
        mDashTime -= deltaTime;
        if (mDashTime <= 0.0f) {
            mIsDashing = false;
        }
    }

    if (mIsWallCrawling) {
        mRigidBodyComponent->SetVelocity(Vector2::UnitY * 2);
    }

    if (mIsFallingThroughPlatform) {
        mFallTime -= deltaTime;
        if (mFallTime <= 0.0f) {
            mIsFallingThroughPlatform = false;
            mColliderComponent->SetEnabled(true);
        }
    }

    // Limit Aeris's position to the camera view
    mPosition.x = Math::Max(mPosition.x, mGame->GetCameraPos().x);
    mPosition.x = Math::Min(mPosition.x,
                            static_cast<float>(
                                (Game::LEVEL_WIDTH - 1) * Game::TILE_SIZE));

    // Kill Aeris if he falls below the screen
    // if (mGame->GetGamePlayState() == Game::GamePlayState::Playing && mPosition.y
    //     > mGame->GetWindowHeight()) {
    //     Kill();
    // }

    // If Aeris is leaving the level, kill him if he enters the castle
    const float castleDoorPos = Game::LEVEL_WIDTH * Game::TILE_SIZE - 10 *
                                Game::TILE_SIZE;

    if (mGame->GetGamePlayState() == Game::GamePlayState::Leaving &&
        mPosition.x >= castleDoorPos) {
        // Stop Aeris and set the game scene to Level 2
        mState = ActorState::Destroy;
        mGame->SetGameScene(Game::GameScene::Level1, 3.5f);

        return;
    }

    ManageAnimations();
}

void Aeris::ManageAnimations()
{
    if (mIsDying) {
        mDrawComponent->SetAnimation("Dead");
    } else if (mIsOnGround && mIsRunning) {
        mDrawComponent->SetAnimation("run");
    } else if (mIsOnGround && !mIsRunning) {
        mDrawComponent->SetAnimation("idle");
    } else if (mIsWallCrawling) {
        mDrawComponent->SetAnimation("crawl");
    } else if (mIsDashing) {
        mDrawComponent->SetAnimation("dash");
    } else if (!mIsOnGround) {
        mDrawComponent->SetAnimation("jump");
    }
}

void Aeris::Kill()
{
    mIsDying = true;
    mGame->SetGamePlayState(Game::GamePlayState::GameOver);
    mDrawComponent->SetAnimation("Dead");

    // Disable collider and rigid body
    mRigidBodyComponent->SetEnabled(false);
    mColliderComponent->SetEnabled(false);

    mGame->GetAudio()->StopAllSounds();
    mGame->GetAudio()->PlaySound("Dead.wav");

    // Remove current level's power-up if it was collected
    mGame->RemoveCurrentLevelPowerUp();

    mGame->ResetGameScene(3.5f); // Reset the game scene after 3 seconds
}

void Aeris::Win(AABBColliderComponent* poleCollider)
{
    mDrawComponent->SetAnimation("win");
    mGame->SetGamePlayState(Game::GamePlayState::LevelComplete);
    GetGame()->SetIsThereCheckPoint(false);

    // Set Aeris velocity to go down
    mRigidBodyComponent->SetVelocity(Vector2::UnitY * 100.0f);
    // 100 pixels per second
    mRigidBodyComponent->SetApplyGravity(false);

    // Disable collider
    poleCollider->SetEnabled(false);

    // Adjust Aeris x position to grab the pole
    mPosition.Set(
        poleCollider->GetOwner()->GetPosition().x + Game::TILE_SIZE / 4.0f,
        mPosition.y);

    mGame->GetAudio()->StopAllSounds();

    // Play level-complete jingle
    mGame->GetAudio()->PlaySound("next_island.wav");

    // Determine next level based on current scene
    Game::GameScene nextLevel;
    switch (mGame->GetCurrentScene()) {
        case Game::GameScene::Level1:
            nextLevel = Game::GameScene::Level2;
            break;
        case Game::GameScene::Level2:
            nextLevel = Game::GameScene::Level3;
            break;
        case Game::GameScene::Level3:
            nextLevel = Game::GameScene::Level4;
            break;
        case Game::GameScene::Level4:
            nextLevel = Game::GameScene::EndGame;
            break;
        default:
            nextLevel = Game::GameScene::MainMenu;
            break;
    }

    // Queue next level transition after short delay
    mGame->SetGameScene(nextLevel, 1.0f);
}

void Aeris::CollectFragment(Fragment* fragment)
{
    mGame->GetAudio()->PlaySound("power_up.wav");
    switch (fragment->GetType()) {
        case Fragment::FragmentType::DoubleJump: {
            mHasUnlockedDoubleJump = true;
            mGame->GetHUD()->onFragmentCollected(Fragment::FragmentType::DoubleJump);
            if (!mGame->IsThereCheckPoint())
                mGame->ToggleFragmentCollected(Fragment::FragmentType::DoubleJump);
            SDL_Log("double jump unlocked");
            break;
        }
        case Fragment::FragmentType::Dash: {
            mHasUnlockedDash = true;
            if (!mGame->IsThereCheckPoint())
                mGame->GetHUD()->onFragmentCollected(Fragment::FragmentType::Dash);
            mGame->ToggleFragmentCollected(Fragment::FragmentType::Dash);
            SDL_Log("dash unlocked");
            break;
        }
        case Fragment::FragmentType::WallJump: {
            mHasUnlockedWallJump = true;
            if (!mGame->IsThereCheckPoint())
                mGame->GetHUD()->onFragmentCollected(Fragment::FragmentType::WallJump);
            mGame->ToggleFragmentCollected(Fragment::FragmentType::WallJump);
            SDL_Log("wall jump unlocked");
            break;
        }
        default: {
        }
    }
    fragment->OnCollected();
}

void Aeris::OnHorizontalCollision(const float minOverlap,
                                  AABBColliderComponent* other)
{
    if (other->GetLayer() == ColliderLayer::Void) {
        Kill();
    }

    if (other->GetLayer() == ColliderLayer::Blocks) {
        auto* block = dynamic_cast<Block*>(other->GetOwner());
        if (block->PlayerCanWallCrawl() && mIsRunning && mHasUnlockedWallJump) {
            mIsWallCrawling = true;
            mJumpCount = 0;
        }
    }

    if (other->GetLayer() == ColliderLayer::Enemy) {
        Kill();
    }

    if (other->GetLayer() == ColliderLayer::Pole) {
        Win(other);
    }

    if (other->GetLayer() == ColliderLayer::Fragment) {
        auto* fragment = dynamic_cast<Fragment*>(other->GetOwner());
        CollectFragment(fragment);
    }
}

void Aeris::OnVerticalCollision(const float minOverlap,
                                AABBColliderComponent* other)
{
    if (other->GetLayer() == ColliderLayer::Void) {
        Kill();
    }

    if (other->GetLayer() == ColliderLayer::Blocks) {
        auto* block = dynamic_cast<Block*>(other->GetOwner());
        if (block->IsOneWayPlatform()) mCanFallThroughPlatform = true;
        else mCanFallThroughPlatform = false;
    }

    if (other->GetLayer() == ColliderLayer::Enemy) {
        Kill();
    }

    if (other->GetLayer() == ColliderLayer::Fragment) {
        auto* fragment = dynamic_cast<Fragment*>(other->GetOwner());
        CollectFragment(fragment);
    }
}
