//
// Created by Lucas N. Ferreira on 28/09/23.
//

#pragma once

#include <string>
#include "Actor.h"

class Block : public Actor
{
public:
    explicit Block(Game* game, const std::string &texturePath);
    void OnVerticalCollision(const float minOverlap, AABBColliderComponent* other) override;
    void OnUpdate(float deltaTime) override;

private:
};
