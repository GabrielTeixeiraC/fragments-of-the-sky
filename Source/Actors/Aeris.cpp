#include "Aeris.h"
#include "Block.h"
#include "Fragment.h"
#include "../Game.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"

Aeris::Aeris(Game* game, const float forwardSpeed, const float jumpSpeed,
             const float dashSpeed)
    : Actor(game)
      , mIsRunning(false)
      , mIsWallCrawling(false)
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
{
    SetActorType(ActorType::Player);

    mRigidBodyComponent = new RigidBodyComponent(this, 1.0f, 14.0f);
    mColliderComponent = new AABBColliderComponent(
        this, 0, 0, Game::TILE_SIZE - 4.0f, Game::TILE_SIZE,
        ColliderLayer::Player);

    mDrawComponent = new DrawAnimatedComponent(this,
                                               "../Assets/Sprites/Mario/Mario.png",
                                               "../Assets/Sprites/Mario/Mario.json");

    mDrawComponent->AddAnimation("Dead", {0});
    mDrawComponent->AddAnimation("idle", {1});
    mDrawComponent->AddAnimation("jump", {2});
    mDrawComponent->AddAnimation("run", {3, 4, 5});
    mDrawComponent->AddAnimation("win", {7});

    mDrawComponent->SetAnimation("idle");
    mDrawComponent->SetAnimFPS(10.0f);
}

void Aeris::OnProcessInput(const uint8_t* state)
{
    if (mGame->GetGamePlayState() != Game::GamePlayState::Playing) return;

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
    mGame->GetAudio()->PlaySound("Jump.wav");
}


void Aeris::OnHandleKeyPress(const int key, const bool isPressed)
{
    if (mGame->GetGamePlayState() != Game::GamePlayState::Playing) return;

    // Jump
    if (key == SDLK_w && isPressed) {
        if (mJumpCount < MAX_JUMP_COUNT) {
            if (mJumpCount == 0) {
                Jump();
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
    if (key == SDLK_LSHIFT && isPressed && mHasUnlockedDash) {
        // TODO: implement dash functionality
        SDL_Log("implement dash");
    }
}

void Aeris::OnUpdate(float deltaTime)
{
    // Check if Aeris is off the ground
    if (mRigidBodyComponent && mRigidBodyComponent->GetVelocity().y != 0) {
        mIsOnGround = false;
    }

    if (mHasQueuedJump) {
        mQueuedJumpTime -= deltaTime;
        if (mQueuedJumpTime <= 0.0f) {
            mHasQueuedJump = false;
            mQueuedJumpTime = 0.0f;
        }
    }

    if (mIsWallCrawling) {
        mRigidBodyComponent->SetVelocity(Vector2::UnitY * 2);
    }

    // Limit Aeris's position to the camera view
    mPosition.x = Math::Max(mPosition.x, mGame->GetCameraPos().x);

    // Kill Aeris if he falls below the screen
    if (mGame->GetGamePlayState() == Game::GamePlayState::Playing && mPosition.y
        > mGame->GetWindowHeight()) {
        Kill();
    }

    // If Aeris is leaving the level, kill him if he enters the castle
    const float castleDoorPos = Game::LEVEL_WIDTH * Game::TILE_SIZE - 10 *
                                Game::TILE_SIZE;

    if (mGame->GetGamePlayState() == Game::GamePlayState::Leaving &&
        mPosition.x >= castleDoorPos) {
        // Stop Aeris and set the game scene to Level 2
        mState = ActorState::Destroy;
        mGame->SetGameScene(Game::GameScene::Level2, 3.5f);

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

    mGame->ResetGameScene(3.5f); // Reset the game scene after 3 seconds
}

void Aeris::Win(AABBColliderComponent* poleCollider)
{
    mDrawComponent->SetAnimation("win");
    mGame->SetGamePlayState(Game::GamePlayState::LevelComplete);

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
}

void Aeris::CollectFragment(Fragment* fragment)
{
    mGame->GetAudio()->PlaySound("Coin.wav");
    switch (fragment->GetType()) {
        case Fragment::FragmentType::DoubleJump: {
            mHasUnlockedDoubleJump = true;
            SDL_Log("double jump unlocked");
            break;
        }
        case Fragment::FragmentType::Dash: {
            mHasUnlockedDash = true;
            SDL_Log("dash unlocked");
            break;
        }
        case Fragment::FragmentType::WallJump: {
            mHasUnlockedWallJump = true;
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
    if (other->GetLayer() == ColliderLayer::Blocks) {
        auto* block = dynamic_cast<Block*>(other->GetOwner());
        if (block->PlayerCanWallCrawl() && mIsRunning && mHasUnlockedWallJump) {
            mIsWallCrawling = true;
            mJumpCount = 0;
        }
    }

    if (other->GetLayer() == ColliderLayer::Enemy) {
        Kill();
    } else if (other->GetLayer() == ColliderLayer::Pole) {
        Win(other);
    } else if (other->GetLayer() == ColliderLayer::Fragment) {
        auto* fragment = dynamic_cast<Fragment*>(other->GetOwner());
        CollectFragment(fragment);
    }
}

void Aeris::OnVerticalCollision(const float minOverlap,
                                AABBColliderComponent* other)
{
    if (other->GetLayer() == ColliderLayer::Enemy) {
        other->GetOwner()->Kill();
        mRigidBodyComponent->SetVelocity(
            Vector2(mRigidBodyComponent->GetVelocity().x, mJumpSpeed / 2.5f));
        mGame->GetAudio()->PlaySound("Stomp.wav");
    } else if (other->GetLayer() == ColliderLayer::Fragment) {
        auto* fragment = dynamic_cast<Fragment*>(other->GetOwner());
        CollectFragment(fragment);
    }
}