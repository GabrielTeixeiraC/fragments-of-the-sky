#pragma once

#include "Actor.h"
#include <string>

class Block : public Actor
{
public:
    explicit Block(Game* game, const std::string& texturePath,
                   const bool isStatic = true,
                   const bool playerCanCrawl = false,
                   const bool isOneWayPlatform = false);

    void OnUpdate(float deltaTime) override;

    void OnVerticalCollision(const float minOverlap,
                             AABBColliderComponent* other) override;

    bool PlayerCanWallCrawl() { return mPlayerCanCrawl; }

    bool IsOneWayPlatform() { return mIsOneWayPlatform; }

private:
    bool mPlayerCanCrawl;
    bool mIsOneWayPlatform;

    class AABBColliderComponent* mColliderComponent;
    class RigidBodyComponent* mRigidBodyComponent;
};