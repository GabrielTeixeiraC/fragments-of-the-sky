#pragma once

#include <SDL.h>
#include "UIElement.h"
#include "../../Utils/Math.h"

class UIRect : public UIElement
{
public:
    UIRect(const Vector2& pos, const Vector2& size,
           const Vector3& color = Vector3(0, 0, 0),
           int alpha = 255, SDL_Renderer* renderer = nullptr);

    ~UIRect();

    void Draw(SDL_Renderer* renderer, const Vector2& screenPos) override;

private:
    int mAlpha;

    SDL_Renderer* mRenderer;
};