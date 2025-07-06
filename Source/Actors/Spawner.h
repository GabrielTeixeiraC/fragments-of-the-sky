#pragma once

#include "../Game.h"
#include "Actor.h"

class Spawner : public Actor
{
public:
    explicit Spawner(Game* game, float spawnDistance, Game::GameScene gameScene);

    void OnUpdate(float deltaTime) override;

private:
    float mSpawnDistance;
    Game::GameScene mGameScene;
};
