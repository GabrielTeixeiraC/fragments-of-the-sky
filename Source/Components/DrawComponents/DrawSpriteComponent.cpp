#include "DrawSpriteComponent.h"
#include "../../Actors/Actor.h"
#include "../../Game.h"

DrawSpriteComponent::DrawSpriteComponent(class Actor* owner,
                                         const std::string& texturePath,
                                         const int width, const int height,
                                         const int drawOrder)
    : DrawComponent(owner, drawOrder)
      , mWidth(width)
      , mHeight(height)
{
    mSpriteSheetSurface = mOwner->GetGame()->GetCachedTexture(texturePath);
}

DrawSpriteComponent::~DrawSpriteComponent()
{
    DrawComponent::~DrawComponent();

    // Don't destroy texture here as it's cached by Game
    // The Game class will handle texture cleanup
    mSpriteSheetSurface = nullptr;
}

void DrawSpriteComponent::Draw(SDL_Renderer* renderer, const Vector3& modColor)
{
    SDL_Rect dstRect = {
        static_cast<int>(mOwner->GetPosition().x - mOwner->GetGame()->
                         GetCameraPos().x),
        static_cast<int>(mOwner->GetPosition().y - mOwner->GetGame()->
                         GetCameraPos().y),
        mWidth,
        mHeight
    };

    SDL_RendererFlip flip = SDL_FLIP_NONE;
    if (mOwner->GetRotation() == Math::Pi) {
        flip = SDL_FLIP_HORIZONTAL;
    }

    SDL_SetTextureBlendMode(mSpriteSheetSurface, SDL_BLENDMODE_BLEND);

    SDL_SetTextureColorMod(mSpriteSheetSurface,
                           static_cast<Uint8>(modColor.x),
                           static_cast<Uint8>(modColor.y),
                           static_cast<Uint8>(modColor.z));

    SDL_RenderCopyEx(renderer, mSpriteSheetSurface, nullptr, &dstRect,
                     0, nullptr, flip);
}