// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
//
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include <SDL2/SDL_stdinc.h>

class Component {
public:
    // Constructor
    // (the lower the update order, the earlier the component updates)
    explicit Component(class Actor *owner, int updateOrder = 100);

    // Destructor
    virtual ~Component();

    // Reinsert this component by delta time
    virtual void Update(float deltaTime);

    // Process input for this component (if needed)
    virtual void ProcessInput(const Uint8 *keyState);

    // Handle key press for this component (if needed)
    virtual void HandleKeyPress(const int key, const bool isPressed);

    int GetUpdateOrder() const { return mUpdateOrder; }
    class Actor *GetOwner() const { return mOwner; }

    class Game *GetGame() const;

    void SetEnabled(const bool enabled) { mIsEnabled = enabled; };
    bool IsEnabled() const { return mIsEnabled; };

protected:
    // Owning actor
    class Actor *mOwner;
    // Reinsert order
    int mUpdateOrder;

    bool mIsEnabled;
};
