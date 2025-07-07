#pragma once

#include "Actor.h"
#include "../Game.h"

class Enemy : public Actor
{
public:
    explicit Enemy(Game* game, Game::GameScene gameScene, float forwardSpeed = 100.0f);

    void OnUpdate(float deltaTime) override;

    void OnHorizontalCollision(const float minOverlap,
                               AABBColliderComponent* other) override;

    void OnVerticalCollision(const float minOverlap,
                             AABBColliderComponent* other) override;

private:
    void ManageAnimations();

    Game::GameScene mGameScene;

    bool mIsMoving;
    bool mMovingRight;  // Direction for Level 3 Scorpion

    float mForwardSpeed;

    class RigidBodyComponent* mRigidBodyComponent;
    class DrawAnimatedComponent* mDrawComponent;
    class AABBColliderComponent* mColliderComponent;
};