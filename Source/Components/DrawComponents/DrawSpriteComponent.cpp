//
// Created by Lucas N. Ferreira on 28/09/23.
//

#include "DrawSpriteComponent.h"
#include "../../Actors/Actor.h"
#include "../../Game.h"

DrawSpriteComponent::DrawSpriteComponent(class Actor* owner, const std::string &texturePath, const int width, const int height, const int drawOrder)
        :DrawComponent(owner, drawOrder)
        ,mWidth(width)
        ,mHeight(height)
{
    mSpriteSheetSurface = mOwner->GetGame()->LoadTexture(texturePath);
}

void DrawSpriteComponent::Draw(SDL_Renderer *renderer)
{
    SDL_Rect destRect = {static_cast<int>(mOwner->GetPosition().x - mOwner->GetGame()->GetCameraPos().x),
                         static_cast<int>(mOwner->GetPosition().y - mOwner->GetGame()->GetCameraPos().y),
                         mWidth, mHeight};
    SDL_RendererFlip flip = SDL_FLIP_NONE;
    if (mOwner->GetRotation() == Math::Pi) {
        flip = SDL_FLIP_HORIZONTAL;
    }

    SDL_RenderCopyEx(renderer, mSpriteSheetSurface, nullptr, &destRect, 0, nullptr, flip);
}