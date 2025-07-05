#include "UIRotatingImage.h"
#include <iostream>
#include <cmath>

UIRotatingImage::UIRotatingImage(SDL_Renderer* renderer, const std::string& imagePath,
                                 const Vector2& pos, const Vector2& size,
                                 const Vector3& color)
    : UIElement(pos, size, color)
    , mRenderer(renderer)
    , mTexture(nullptr)
    , mRotation(0.0f)
{
    // Load the texture
    SDL_Surface* surface = IMG_Load(imagePath.c_str());
    if (surface) {
        mTexture = SDL_CreateTextureFromSurface(mRenderer, surface);
        SDL_FreeSurface(surface);
    } else {
        std::cerr << "Failed to load image: " << imagePath << std::endl;
    }
}

UIRotatingImage::~UIRotatingImage()
{
    if (mTexture) {
        SDL_DestroyTexture(mTexture);
    }
}

void UIRotatingImage::Draw(SDL_Renderer* renderer, const Vector2& screenPos)
{
    if (!mTexture) {
        return;
    }

    // Set up the destination rectangle
    SDL_Rect dstRect = {
        static_cast<int>(mPosition.x + screenPos.x),
        static_cast<int>(mPosition.y + screenPos.y),
        static_cast<int>(mSize.x),
        static_cast<int>(mSize.y)
    };

    // Convert rotation from radians to degrees
    double rotationDegrees = mRotation * 180.0 / M_PI;

    // Draw the rotated texture
    SDL_RenderCopyEx(renderer, mTexture, nullptr, &dstRect, rotationDegrees, nullptr, SDL_FLIP_NONE);
} 