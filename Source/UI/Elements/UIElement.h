#pragma once

#include <SDL.h>
#include "../../Utils/Math.h"

class UIElement
{
public:
    UIElement(const Vector2& pos, const Vector2& size, const Vector3& color);

    // Getters/setters
    const Vector2& GetPosition() const { return mPosition; }
    void SetPosition(const Vector2& pos) { mPosition = pos; }

    const Vector2& GetSize() const { return mSize; }
    void SetSize(const Vector2& size) { mSize = size; }

    const Vector3& GetColor() const { return mColor; }
    void SetColor(const Vector3& color) { mColor = color; }

    virtual void Draw(SDL_Renderer* renderer, const Vector2& screenPos)
    {
    };

protected:
    Vector2 mPosition;
    Vector2 mSize;
    Vector3 mColor;
};