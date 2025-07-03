#include "UIText.h"
#include "UIFont.h"

UIText::UIText(const std::string& text, class UIFont* font, int pointSize,
               const unsigned wrapLength,
               const Vector2& pos, const Vector2& size, const Vector3& color)
    : UIElement(pos, size, color)
      , mFont(font)
      , mPointSize(pointSize)
      , mWrapLength(wrapLength)
      , mTextTexture(nullptr)
{
    SetText(text);
}

UIText::~UIText()
{
}

void UIText::SetText(const std::string& text)
{
    if (mTextTexture) {
        SDL_DestroyTexture(mTextTexture);
        mTextTexture = nullptr;
    }

    mTextTexture = mFont->RenderText(text, mColor, static_cast<int>(mPointSize),
                                     mWrapLength);
    if (mTextTexture) {
        mText = text;
    } else {
        SDL_Log("Failed to set text: %s", text.c_str());
    }
}

void UIText::Draw(SDL_Renderer* renderer, const Vector2& screenPos)
{
    int texW = 0;
    int texH = 0;
    SDL_QueryTexture(mTextTexture, nullptr, nullptr, &texW, &texH);

    SDL_Rect titleQuad;
    titleQuad.x = static_cast<int>(mPosition.x + screenPos.x);
    titleQuad.y = static_cast<int>(mPosition.y + screenPos.y);
    titleQuad.w = texW;
    titleQuad.h = texH;

    SDL_RenderCopyEx(renderer, mTextTexture, nullptr, &titleQuad, 0.0, nullptr,
                     SDL_FLIP_NONE);
}