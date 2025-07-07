#include "Fragment.h"

#include "../Game.h"
#include "../Components/DrawComponents/DrawSpriteComponent.h"
#include "../Components/ColliderComponents/AABBColliderComponent.h"

Fragment::Fragment(Game* game, FragmentType type)
    : Actor(game)
      , mFragmentType(type)
{
    switch (type) {
        case FragmentType::DoubleJump: {
            new DrawSpriteComponent(
                this, "../Assets/Sprites/Fragments/doubleJump.png", 32, 32, 1);
        }
        case FragmentType::Dash: {
            new DrawSpriteComponent(
                this, "../Assets/Sprites/Fragments/dash.png", 32, 32, 1);
        }
        case FragmentType::WallJump: {
            new DrawSpriteComponent(
                this, "../Assets/Sprites/Fragments/wall.png", 32, 32, 1);
        }
    }
    new AABBColliderComponent(this, 2, 2, 28, 28, ColliderLayer::Fragment);
}

void Fragment::OnCollected()
{
    GetComponent<AABBColliderComponent>()->SetEnabled(false);
    SetState(ActorState::Destroy);
    GetGame()->SetCheckPointPosition(GetPosition());
    GetGame()->SetIsThereCheckPoint(true);
}
