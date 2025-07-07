#include "Enemy.h"

#include "Aeris.h"
#include "../Game.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"
#include "../Utils/Random.h"

Enemy::Enemy(Game* game, Game::GameScene gameScene, float forwardSpeed)
    : Actor(game)
      , mForwardSpeed(forwardSpeed)
      , mIsMoving(false)
      , mGameScene(gameScene)
      , mMovingRight(true)  // Initialize direction for Level 3
{
    mRigidBodyComponent = new RigidBodyComponent(this, 1.0f);

    switch (gameScene) {
        case Game::GameScene::Level1: {
            SDL_Log("level1 enemy");
            mColliderComponent = new AABBColliderComponent(this, 0, 0,
                                                   62,
                                                   30,
                                                   ColliderLayer::Enemy);
            mDrawComponent = new DrawAnimatedComponent(this,
                                               "../Assets/Sprites/Snake/Snake.png",
                                               "../Assets/Sprites/Snake/Snake.json");
            mDrawComponent->AddAnimation("idle", {0, 1, 2, 3});
            mDrawComponent->AddAnimation("walk", {4, 5, 6, 7});
            mDrawComponent->SetAnimation("idle");
            mDrawComponent->SetAnimFPS(5.0f);
            break;
        }
        case Game::GameScene::Level3: {
            SDL_Log("level3 enemy");
            mColliderComponent = new AABBColliderComponent(this, 0, 20,
                                                   48,
                                                   26,
                                                   ColliderLayer::Enemy);
            // Start moving right for Level 3
            mRigidBodyComponent->SetVelocity(Vector2(mForwardSpeed, 0));
            mDrawComponent = new DrawAnimatedComponent(this,
                                               "../Assets/Sprites/Scorpio/Scorpio.png",
                                               "../Assets/Sprites/Scorpio/Scorpio.json");
            mDrawComponent->AddAnimation("idle", {0, 1, 2, 3});
            mDrawComponent->AddAnimation("walk", {4, 5, 6, 7});
            mDrawComponent->SetAnimation("walk");  // Start walking
            mDrawComponent->SetAnimFPS(5.0f);
            break;
        }
        case Game::GameScene::Level4: {
            mColliderComponent = new AABBColliderComponent(this, 0, 20,
                                                   48,
                                                   26,
                                                   ColliderLayer::Enemy);
            mRigidBodyComponent->SetVelocity(Vector2(-mForwardSpeed, 0));
            mDrawComponent = new DrawAnimatedComponent(this,
                                                "../Assets/Sprites/Scorpio/Scorpio.png",
                                                "../Assets/Sprites/Scorpio/Scorpio.json");
            mDrawComponent->AddAnimation("idle", {0, 1, 2, 3});
            mDrawComponent->AddAnimation("walk", {4, 5, 6, 7});
            mDrawComponent->SetAnimation("walk");  // Start walking
            mDrawComponent->SetAnimFPS(5.0f);
            break;
        }
        default: break;
    }
}

void Enemy::OnUpdate(float deltaTime)
{
    if (mGameScene == Game::GameScene::Level1) {
        // Level 1 Snake follows the player
        Vector2 aerisPosition = mGame->GetAeris()->GetPosition();
        if (Math::NearZero(aerisPosition.x - GetPosition().x, 5.0f)) {
            mRigidBodyComponent->SetVelocity(Vector2::Zero);
            mIsMoving = false;
        } else {
            mIsMoving = true;
            if (aerisPosition.x > GetPosition().x) {
                mRigidBodyComponent->SetVelocity(
                    Vector2(mForwardSpeed, mRigidBodyComponent->GetVelocity().y));
                SetRotation(Math::Pi);
            } else {
                mRigidBodyComponent->SetVelocity(
                    Vector2(-mForwardSpeed, mRigidBodyComponent->GetVelocity().y));
                SetRotation(0);
            }
        }
    } else if (mGameScene == Game::GameScene::Level3 || mGameScene == Game::GameScene::Level4) {
        // Level 3 Scorpion moves in current direction (changes direction only on wall collision)
        if (mMovingRight) {
            mRigidBodyComponent->SetVelocity(Vector2(mForwardSpeed, mRigidBodyComponent->GetVelocity().y));
            SetRotation(Math::Pi);
        } else {
            mRigidBodyComponent->SetVelocity(Vector2(-mForwardSpeed, mRigidBodyComponent->GetVelocity().y));
            SetRotation(0);
        }
        mIsMoving = true;
    }
    // Level 4 movement is handled in constructor and doesn't need updates

    ManageAnimations();
}

void Enemy::ManageAnimations()
{
    if (mIsMoving) {
        mDrawComponent->SetAnimation("walk");
    } else {
        mDrawComponent->SetAnimation("idle");
    }
}

void Enemy::OnHorizontalCollision(const float minOverlap,
                                  AABBColliderComponent* other)
{
    if (other->GetLayer() == ColliderLayer::Player) {
        other->GetOwner()->Kill();
    }

    if (other->GetLayer() == ColliderLayer::Void) {
        mState = ActorState::Destroy;
    }

    // Level 3 Scorpion changes direction when hitting walls
    if ((mGameScene == Game::GameScene::Level3 || mGameScene == Game::GameScene::Level4) && (other->GetLayer() == ColliderLayer::Blocks || other->GetLayer() == ColliderLayer::Enemy)) {
        mMovingRight = !mMovingRight;  // Reverse direction
    }
}

void Enemy::OnVerticalCollision(const float minOverlap,
                                AABBColliderComponent* other)
{
    if (other->GetLayer() == ColliderLayer::Player) {
        other->GetOwner()->Kill();
    }

    if (other->GetLayer() == ColliderLayer::Void) {
        mState = ActorState::Destroy;
    }
}
