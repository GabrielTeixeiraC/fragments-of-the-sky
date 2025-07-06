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
{
    mRigidBodyComponent = new RigidBodyComponent(this, 1.0f);
    mRigidBodyComponent->SetVelocity(Vector2(-mForwardSpeed, 0.0f));

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
            mColliderComponent = new AABBColliderComponent(this, 0, 0,
                                                   66,
                                                   46,
                                                   ColliderLayer::Enemy);
            mDrawComponent = new DrawAnimatedComponent(this,
                                               "../Assets/Sprites/Scorpio/Scorpio.png",
                                               "../Assets/Sprites/Scorpio/Scorpio.json");
            mDrawComponent->AddAnimation("idle", {0, 1, 2, 3});
            mDrawComponent->AddAnimation("walk", {4, 5, 6, 7});
            mDrawComponent->SetAnimation("idle");
            mDrawComponent->SetAnimFPS(5.0f);
            break;
        }
        case Game::GameScene::Level4: {
            // TODO: implementar o passarin
            SDL_Log("level4 enemy");
            mDrawComponent = new DrawAnimatedComponent(this,
                                               "../Assets/Sprites/Goomba/Goomba.png",
                                               "../Assets/Sprites/Goomba/Goomba.json");
            break;
        }
        default: break;
    }
}

void Enemy::OnUpdate(float deltaTime)
{
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