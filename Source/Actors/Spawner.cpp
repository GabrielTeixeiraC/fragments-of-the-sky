#include "Spawner.h"
#include "../Game.h"
#include "Aeris.h"
#include "Enemy.h"

Spawner::Spawner(Game* game, float spawnDistance, Game::GameScene gameScene)
    : Actor(game)
      , mSpawnDistance(spawnDistance)
      , mGameScene(gameScene)
{
}

void Spawner::OnUpdate(float deltaTime)
{
    if (abs(GetGame()->GetAeris()->GetPosition().x - GetPosition().x) <
        mSpawnDistance) {
        auto enemy = new Enemy(GetGame(), mGameScene);
        enemy->SetPosition(GetPosition());
        mState = ActorState::Destroy;
    }
}