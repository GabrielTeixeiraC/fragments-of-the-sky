#pragma once

#include "Actor.h"
#include <string>

class Block : public Actor
{
public:
    explicit Block(Game* game, const std::string& texturePath,
                   const bool isStatic = true,
                   const bool playerCanCrawl = true);

    void OnVerticalCollision(const float minOverlap,
                             AABBColliderComponent* other) override;

    bool PlayerCanWallCrawl() { return mPlayerCanCrawl; }

private:
    bool mPlayerCanCrawl;

    class AABBColliderComponent* mColliderComponent;
    class RigidBodyComponent* mRigidBodyComponent;
};