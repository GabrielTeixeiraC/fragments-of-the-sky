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
                this, "../Assets/Sprites/Collectables/Mushroom.png", 32, 32, 1);
        }
        case FragmentType::Dash: {
            new DrawSpriteComponent(
                this, "../Assets/Sprites/Goomba/Walk0.png", 32, 32, 1);
        }
        case FragmentType::WallJump: {
            new DrawSpriteComponent(
                this, "../Assets/Sprites/Goomba/Walk1.png", 32, 32, 1);
        }
    }
    new AABBColliderComponent(this, 2, 2, 28, 28, ColliderLayer::Fragment);
}

void Fragment::OnCollected()
{
    GetComponent<AABBColliderComponent>()->SetEnabled(false);
    SetState(ActorState::Destroy);
}