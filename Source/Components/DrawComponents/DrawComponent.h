//
// Created by Lucas N. Ferreira on 03/08/23.
//

#pragma once
#include "../Component.h"
#include "../../Math.h"
#include <vector>
#include <SDL2/SDL.h>

class DrawComponent : public Component
{
public:
    // (Lower draw order corresponds with further back)
    explicit DrawComponent(class Actor* owner, int drawOrder = 100);
    ~DrawComponent() override;

    virtual void Draw(SDL_Renderer* renderer);

    bool IsVisible() const { return mIsVisible; }
    void SetIsVisible(const bool isVisible) { mIsVisible = isVisible; }

    int GetDrawOrder() const { return mDrawOrder; }

protected:
    bool mIsVisible;
    int mDrawOrder;
};
