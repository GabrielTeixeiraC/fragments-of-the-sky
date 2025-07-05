#pragma once

#include <string>
#include <SDL_image.h>
#include "UIElement.h"

class UIRotatingImage : public UIElement
{
public:
    UIRotatingImage(SDL_Renderer* renderer, const std::string& imagePath,
                    const Vector2& pos = Vector2::Zero,
                    const Vector2& size = Vector2(100.f, 100.f),
                    const Vector3& color = Color::White);

    ~UIRotatingImage();

    void Draw(SDL_Renderer* renderer, const Vector2& screenPos) override;

    // Rotation functions
    void SetRotation(float rotation) { mRotation = rotation; }
    float GetRotation() const { return mRotation; }

private:
    SDL_Renderer* mRenderer;
    SDL_Texture* mTexture;
    float mRotation; // Rotation in radians
}; 