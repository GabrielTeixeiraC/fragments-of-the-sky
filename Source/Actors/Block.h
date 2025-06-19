#pragma once

#include "Actor.h"
#include <string>

class Block : public Actor
{
public:
    explicit Block(Game* game, const std::string& texturePath,
                   const bool isStatic = true,
                   const bool mIsWallJumpable = true);

    void SetPosition(const Vector2& position)
    {
        Actor::SetPosition(position);
        mOriginalPosition.Set(position.x, position.y);
    }

    void OnUpdate(float deltaTime) override;

    void OnBump();

    void OnVerticalCollision(const float minOverlap,
                             AABBColliderComponent* other) override;

    bool IsWallJumpable() { return mIsWallJumpable; }

private:
    const int BUMP_FORCE = 200;

    bool mIsWallJumpable;

    Vector2 mOriginalPosition;

    class AABBColliderComponent* mColliderComponent;
    class RigidBodyComponent* mRigidBodyComponent;
};