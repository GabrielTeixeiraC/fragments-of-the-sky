#include "FlagBlock.h"
#include "../Game.h"
#include "../Components/DrawComponents/DrawSpriteComponent.h"
#include "../Components/ColliderComponents/AABBColliderComponent.h"

FlagBlock::FlagBlock(Game* game)
    : Actor(game)
{
    // Use BlockJ texture like other blocks
    new DrawSpriteComponent(this,
                            "../Assets/Sprites/Blocks/BlockJ.png",
                            Game::TILE_SIZE,
                            Game::TILE_SIZE,
                            10);

    // Static collider so it doesn't move; layer Pole so Aeris triggers Win
    new AABBColliderComponent(this,
                              0,
                              0,
                              Game::TILE_SIZE,
                              Game::TILE_SIZE,
                              ColliderLayer::Pole,
                              true); // isStatic

    // Rigid body (mass 1, zero gravity) but not affected by gravity
    new RigidBodyComponent(this, 1.0f, 0.0f, false);
} 