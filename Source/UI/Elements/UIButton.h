#pragma once

#include <functional>
#include <string>
#include <SDL.h>
#include "UIFont.h"
#include "UIText.h"
#include "UIElement.h"
#include "../../Utils/Math.h"

class UIButton : public UIElement
{
public:
    // If imagePath is non-empty, the button will draw that texture instead of a plain rectangle.
    UIButton(const std::string& text, class UIFont* font,
             std::function<void()> onClick,
             const Vector2& pos, const Vector2& size,
             const Vector3& color = Vector3(30, 30, 30),
             int pointSize = 16, const unsigned wrapLength = 1024,
             const Vector2& textPos = Vector2::Zero,
             const Vector2& textSize = Vector2(140.f, 20.0f),
             const Vector3& textColor = Color::White,
             SDL_Renderer* renderer = nullptr,
             const std::string& imagePath = "");

    ~UIButton();

    // Set the name of the button
    void SetText(const std::string& text);

    void Draw(SDL_Renderer* renderer, const Vector2& screenPos) override;

    void SetHighlighted(bool sel) { mHighlighted = sel; }
    bool GetHighlighted() const { return mHighlighted; }

    // Returns true if the point is within the button's bounds
    bool ContainsPoint(const Vector2& pt) const;

    // Called when button is clicked
    void OnClick();

private:
    // Callback funtion
    std::function<void()> mOnClick;

    // Button name
    UIText mText;

    // Check if the button is highlighted
    bool mHighlighted;

    // Optional image background
    SDL_Renderer* mRenderer;
    SDL_Texture* mTexture;
    bool mUseImage;
};