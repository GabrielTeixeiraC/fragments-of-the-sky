#include "UIButton.h"

UIButton::UIButton(const std::string& text, class UIFont* font,
                   std::function<void()> onClick,
                   const Vector2& pos, const Vector2& size,
                   const Vector3& color,
                   int pointSize, unsigned wrapLength,
                   const Vector2& textPos, const Vector2& textSize,
                   const Vector3& textColor)
    : UIElement(pos, size, color)
      , mOnClick(onClick)
      , mHighlighted(false)
      , mText(text, font, pointSize, wrapLength, textPos, textSize, textColor)
{
}

UIButton::~UIButton()
{
}


void UIButton::Draw(SDL_Renderer* renderer, const Vector2& screenPos)
{
    SDL_Rect titleQuad;
    titleQuad.x = static_cast<int>(mPosition.x + screenPos.x);
    titleQuad.y = static_cast<int>(mPosition.y + screenPos.y);
    titleQuad.w = static_cast<int>(mSize.x);
    titleQuad.h = static_cast<int>(mSize.y);

    if (mHighlighted) {
        SDL_SetRenderDrawColor(renderer, 200, 100, 0, 255); // Laranja
        SDL_RenderFillRect(renderer, &titleQuad);
    }

    Vector2 textPos;
    textPos.x = mPosition.x + (mSize.x / 2.0f) - (mText.GetSize().x / 2.0f);
    textPos.y = mPosition.y + (mSize.y / 2.0f) - (mText.GetSize().y / 2.0f);
    mText.Draw(renderer, textPos);
}

void UIButton::OnClick()
{
    if (mOnClick != nullptr) {
        mOnClick();
    }
}