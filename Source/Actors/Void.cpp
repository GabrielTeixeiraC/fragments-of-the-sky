#include "Void.h"
#include "../Game.h"
#include "Aeris.h"

Void::Void(Game* game)
    : Actor(game)
{
    new AABBColliderComponent(this,
                              0,
                              0,
                              Game::TILE_SIZE,
                              Game::TILE_SIZE,
                              ColliderLayer::Void,
                              true);
}