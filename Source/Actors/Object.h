#pragma once

#include "Actor.h"
#include <string>

class Object : public Actor {
public:
    explicit Object(Game* game, const std::string& texturePath, int width, int height);
};
