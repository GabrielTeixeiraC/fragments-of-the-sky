#pragma once

#include <string>
#include <SDL_image.h>
#include "UIElement.h"

class UIImage : public UIElement
{
public:
    UIImage(SDL_Renderer* renderer, const std::string& imagePath,
            const Vector2& pos = Vector2::Zero,
            const Vector2& size = Vector2(100.f, 100.f),
            const Vector3& color = Color::White);

    ~UIImage();

    void Draw(SDL_Renderer* renderer, const Vector2& screenPos) override;

private:
    SDL_Renderer* mRenderer;
    SDL_Texture* mTexture; // Texture for the image
};