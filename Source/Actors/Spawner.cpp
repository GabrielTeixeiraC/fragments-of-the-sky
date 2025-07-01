#include "Spawner.h"
#include "../Game.h"
#include "Aeris.h"
#include "Enemy.h"

Spawner::Spawner(Game* game, float spawnDistance)
    : Actor(game)
      , mSpawnDistance(spawnDistance)
{
}

void Spawner::OnUpdate(float deltaTime)
{
    if (abs(GetGame()->GetAeris()->GetPosition().x - GetPosition().x) <
        mSpawnDistance) {
        auto enemy = new Enemy(GetGame());
        enemy->SetPosition(GetPosition());
        mState = ActorState::Destroy;
    }
}