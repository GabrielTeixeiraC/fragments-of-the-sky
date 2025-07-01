#include "Enemy.h"

#include "Aeris.h"
#include "../Game.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"
#include "../Utils/Random.h"

Enemy::Enemy(Game* game, float forwardSpeed)
    : Actor(game)
      , mForwardSpeed(forwardSpeed)
      , mIsMoving(false)
{
    mRigidBodyComponent = new RigidBodyComponent(this, 1.0f);
    mRigidBodyComponent->SetVelocity(Vector2(-mForwardSpeed, 0.0f));

    mColliderComponent = new AABBColliderComponent(this, 0, 0,
                                                   Game::TILE_SIZE,
                                                   Game::TILE_SIZE,
                                                   ColliderLayer::Enemy);

    mDrawComponent = new DrawAnimatedComponent(this,
                                               "../Assets/Sprites/Goomba/Goomba.png",
                                               "../Assets/Sprites/Goomba/Goomba.json");

    mDrawComponent->AddAnimation("Dead", {0});
    mDrawComponent->AddAnimation("Idle", {1});
    mDrawComponent->AddAnimation("walk", {1, 2});
    mDrawComponent->SetAnimation("walk");
    mDrawComponent->SetAnimFPS(5.0f);
}

void Enemy::OnUpdate(float deltaTime)
{
    Vector2 aerisPosition = GetGame()->GetAeris()->GetPosition();
    if (Math::NearZero(aerisPosition.x - GetPosition().x, 4.0f)) {
        mRigidBodyComponent->SetVelocity(Vector2::Zero);
    } else {
        if (aerisPosition.x > GetPosition().x) {
            mRigidBodyComponent->SetVelocity(
                Vector2(mForwardSpeed, mRigidBodyComponent->GetVelocity().y));
        } else {
            mRigidBodyComponent->SetVelocity(
                Vector2(-mForwardSpeed, mRigidBodyComponent->GetVelocity().y));
        }
    }

    if (GetPosition().y > GetGame()->GetWindowHeight()) {
        mState = ActorState::Destroy;
    }
}

void Enemy::OnHorizontalCollision(const float minOverlap,
                                  AABBColliderComponent* other)
{
    if (other->GetLayer() == ColliderLayer::Player) {
        other->GetOwner()->Kill();
    }
}

void Enemy::OnVerticalCollision(const float minOverlap,
                                AABBColliderComponent* other)
{
    if (other->GetLayer() == ColliderLayer::Player) {
        other->GetOwner()->Kill();
    }
}