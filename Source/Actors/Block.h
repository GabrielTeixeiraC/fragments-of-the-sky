#pragma once

#include "Actor.h"
#include <string>

class Block : public Actor
{
public:
    explicit Block(Game* game, const std::string& texturePath,
                   const bool isStatic = true,
                   const bool mIsWallJumpable = true);

    void OnVerticalCollision(const float minOverlap,
                             AABBColliderComponent* other) override;

    bool IsWallJumpable() { return mIsWallJumpable; }

private:
    bool mIsWallJumpable;

    class AABBColliderComponent* mColliderComponent;
    class RigidBodyComponent* mRigidBodyComponent;
};