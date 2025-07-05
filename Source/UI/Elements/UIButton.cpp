#include "UIButton.h"
#include <SDL_image.h>

UIButton::UIButton(const std::string& text, class UIFont* font,
                   std::function<void()> onClick,
                   const Vector2& pos, const Vector2& size,
                   const Vector3& color,
                   int pointSize, unsigned wrapLength,
                   const Vector2& textPos, const Vector2& textSize,
                   const Vector3& textColor,
                   SDL_Renderer* renderer,
                   const std::string& imagePath)
    : UIElement(pos, size, color)
      , mOnClick(onClick)
      , mHighlighted(false)
      , mText(text, font, pointSize, wrapLength, textPos, textSize, textColor)
      , mRenderer(renderer)
      , mTexture(nullptr)
      , mUseImage(false)
{
    if (mRenderer && !imagePath.empty()) {
        SDL_Surface* surface = IMG_Load(imagePath.c_str());
        if (surface) {
            mTexture = SDL_CreateTextureFromSurface(mRenderer, surface);
            SDL_FreeSurface(surface);
            if (mTexture) {
                mUseImage = true;
            }
        }
    }
}

UIButton::~UIButton()
{
    if (mTexture) {
        SDL_DestroyTexture(mTexture);
        mTexture = nullptr;
    }
}


void UIButton::Draw(SDL_Renderer* renderer, const Vector2& screenPos)
{
    SDL_Rect titleQuad;
    titleQuad.x = static_cast<int>(mPosition.x + screenPos.x);
    titleQuad.y = static_cast<int>(mPosition.y + screenPos.y);
    titleQuad.w = static_cast<int>(mSize.x);
    titleQuad.h = static_cast<int>(mSize.y);

    if (mUseImage && mTexture) {
        SDL_RenderCopy(renderer, mTexture, nullptr, &titleQuad);
    }

    // Draw highlight overlay if selected
    if (mHighlighted) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // white border

        // 1px outside
        SDL_Rect outer1 = {titleQuad.x - 1, titleQuad.y - 1, titleQuad.w + 2,
                           titleQuad.h + 2};
        SDL_RenderDrawRect(renderer, &outer1);

        // Another 1px outside to make it 2px thicker outward (total thickness now 4px visual)
        SDL_Rect outer2 = {titleQuad.x - 2, titleQuad.y - 2, titleQuad.w + 4,
                           titleQuad.h + 4};
        SDL_RenderDrawRect(renderer, &outer2);
    }

    // Draw text on top (optional)
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