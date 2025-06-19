#include "Collectible.h"

#include "../Game.h"
#include "../Components/DrawComponents/DrawSpriteComponent.h"
#include "../Components/ColliderComponents/AABBColliderComponent.h"

Collectible::Collectible(Game* game, CollectibleType type)
    : Actor(game)
      , mCollectibleType(type)
{
    switch (type) {
        case CollectibleType::Coin: {
            new DrawSpriteComponent(
                this, "../Assets/Sprites/Collectables/Coin.png", 32, 32, 1);
        }
        default: {
            new DrawSpriteComponent(
                this, "../Assets/Sprites/Collectables/Coin.png", 32, 32, 1);
        }
    }
    new AABBColliderComponent(this, 2, 2, 28, 28, ColliderLayer::Collectable);
}

void Collectible::OnCollected()
{
    mGame->IncreaseCoins();
    GetComponent<AABBColliderComponent>()->SetEnabled(false);
    SetState(ActorState::Destroy);
}