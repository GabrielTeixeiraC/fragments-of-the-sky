#include "Block.h"
#include "../Game.h"
#include "../Components/DrawComponents/DrawSpriteComponent.h"
#include "../Components/ColliderComponents/AABBColliderComponent.h"

Block::Block(Game* game, const std::string& texturePath, const bool isStatic,
             const bool isWallJumpable)
    : Actor(game)
      , mIsWallJumpable(isWallJumpable)
{
    new DrawSpriteComponent(this, texturePath, Game::TILE_SIZE, Game::TILE_SIZE,
                            10);
    mColliderComponent = new AABBColliderComponent(
        this, 0, 0, Game::TILE_SIZE, Game::TILE_SIZE, ColliderLayer::Blocks,
        isStatic);
    mRigidBodyComponent = new RigidBodyComponent(this, 1.0f, 0.0f, false);
}

void Block::OnVerticalCollision(const float minOverlap,
                                AABBColliderComponent* other)
{
}