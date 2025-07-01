#pragma once

#include "Actor.h"

class Enemy : public Actor
{
public:
    explicit Enemy(Game* game, float forwardSpeed = 100.0f);

    void OnUpdate(float deltaTime) override;

    void OnHorizontalCollision(const float minOverlap,
                               AABBColliderComponent* other) override;

    void OnVerticalCollision(const float minOverlap,
                             AABBColliderComponent* other) override;

private:
    float mForwardSpeed;

    class RigidBodyComponent* mRigidBodyComponent;
    class DrawAnimatedComponent* mDrawComponent;
    class AABBColliderComponent* mColliderComponent;
};