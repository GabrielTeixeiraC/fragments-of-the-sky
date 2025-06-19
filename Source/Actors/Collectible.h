#pragma once

#include "Actor.h"

class Collectible : public Actor
{
public:
    enum class CollectibleType { Coin, PowerUp };

    Collectible(class Game* game, CollectibleType type);

    CollectibleType GetType() const { return mCollectibleType; }

    void OnCollected();

private:
    CollectibleType mCollectibleType;
};