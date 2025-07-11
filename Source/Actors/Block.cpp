#include "Block.h"

#include "Aeris.h"
#include "../Game.h"
#include "../Components/DrawComponents/DrawSpriteComponent.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"
#include "../Components/ColliderComponents/AABBColliderComponent.h"

Block::Block(Game* game, const std::string& texturePath, const bool isStatic,
             const bool playerCanCrawl, const bool isOneWayPlatform)
    : Actor(game)
      , mPlayerCanCrawl(playerCanCrawl)
      , mIsOneWayPlatform(isOneWayPlatform)
{
    new DrawSpriteComponent(this, texturePath, Game::TILE_SIZE, Game::TILE_SIZE,
                            100);
    mColliderComponent = new AABBColliderComponent(
        this, 0, 0, Game::TILE_SIZE, Game::TILE_SIZE, ColliderLayer::Blocks,
        isStatic);
}

void Block::OnUpdate(float deltaTime)
{
    Vector2 aerisPosition = GetGame()->GetAeris()->GetPosition();
    if (mIsOneWayPlatform && aerisPosition.y + Game::TILE_SIZE > GetPosition().
        y) {
        mColliderComponent->SetEnabled(false);
    } else {
        mColliderComponent->SetEnabled(true);
    }
}

void Block::OnVerticalCollision(const float minOverlap,
                                AABBColliderComponent* other)
{
}