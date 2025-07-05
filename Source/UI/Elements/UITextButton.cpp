#include "UITextButton.h"

UITextButton::UITextButton(const std::string& text, class UIFont* font,
                           std::function<void()> onClick,
                           const Vector2& pos, const Vector2& size,
                           const Vector3& color,
                           int pointSize, unsigned wrapLength,
                           const Vector2& textPos, const Vector2& textSize,
                           const Vector3& textColor,
                           SDL_Renderer* renderer)
    : UIElement(pos, size, color)
      , mOnClick(onClick)
      , mHighlighted(false)
      , mText(text, font, pointSize, wrapLength, textPos, textSize, textColor)
      , mRenderer(renderer)
      , mTexture(nullptr)
      , mTextContent(text)
{
}

UITextButton::~UITextButton()
{
}


void UITextButton::Draw(SDL_Renderer* renderer, const Vector2& screenPos)
{
    Vector2 textPos;
    textPos.x = mPosition.x + (mSize.x / 2.0f) - (mText.GetSize().x / 2.0f);
    textPos.y = mPosition.y + (mSize.y / 2.0f) - (mText.GetSize().y / 2.0f);
    if (mHighlighted) {
        mText.SetColor(Color::LightBlue);
        mText.SetText(mTextContent);
        mText.Draw(renderer, textPos);
    } else {
        mText.SetColor(Color::White);
        mText.SetText(mTextContent);
        mText.Draw(renderer, textPos);
    }
}

void UITextButton::OnClick()
{
    if (mOnClick != nullptr) {
        mOnClick();
    }
}