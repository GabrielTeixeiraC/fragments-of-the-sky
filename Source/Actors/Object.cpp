#include "Object.h"
#include "../Game.h"
#include "../Components/DrawComponents/DrawSpriteComponent.h"

Object::Object(Game* game, const std::string& texturePath, int width, int height)
    : Actor(game)
{
    new DrawSpriteComponent(this, texturePath, width, height,
                            100);
}