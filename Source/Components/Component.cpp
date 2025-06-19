// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
//
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Component.h"
#include "../Actors/Actor.h"

Component::Component(Actor* owner, int updateOrder)
    : mOwner(owner)
      , mUpdateOrder(updateOrder)
      , mIsEnabled(true)
{
    // Add to actor's vector of components
    mOwner->AddComponent(this);
}

Component::~Component()
{
}

void Component::Update(float deltaTime)
{
}

void Component::ProcessInput(const Uint8* keyState)
{
}

void Component::HandleKeyPress(const int key, bool isPressed)
{
}

class Game* Component::GetGame() const
{
    return mOwner->GetGame();
}