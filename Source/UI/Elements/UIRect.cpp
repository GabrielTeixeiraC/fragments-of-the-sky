#include "UIRect.h"

UIRect::UIRect(const Vector2& pos, const Vector2& size,
               const Vector3& color, int alpha, SDL_Renderer* renderer)
    : UIElement(pos, size, color)
      , mRenderer(renderer)
      , mAlpha(alpha)
{
}

UIRect::~UIRect()
{
}

void UIRect::Draw(SDL_Renderer* renderer, const Vector2& screenPos)
{
    SDL_Rect rect;
    rect.x = static_cast<int>(mPosition.x + screenPos.x);
    rect.y = static_cast<int>(mPosition.y + screenPos.y);
    rect.w = static_cast<int>(mSize.x);
    rect.h = static_cast<int>(mSize.y);
    SDL_SetRenderDrawColor(renderer, mColor.x, mColor.y, mColor.z, mAlpha);
    SDL_RenderFillRect(renderer, &rect);
}