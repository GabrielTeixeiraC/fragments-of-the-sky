#include "DrawComponent.h"
#include "../../Actors/Actor.h"
#include "../../Game.h"

DrawComponent::DrawComponent(class Actor* owner, int drawOrder)
    : Component(owner)
      , mDrawOrder(drawOrder)
      , mIsVisible(true)
{
    //    mOwner->GetGame()->AddDrawable(this);
}

DrawComponent::~DrawComponent()
{
    //    mOwner->GetGame()->RemoveDrawable(this);
}


void DrawComponent::Draw(SDL_Renderer* renderer, const Vector3& modColor)
{
}