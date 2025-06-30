#pragma once

#include <string>
#include <unordered_map>
#include <SDL2/SDL_ttf.h>
#include "../../Utils/Math.h"

class UIFont
{
public:
    UIFont(SDL_Renderer* renderer);

    ~UIFont();

    // Start/unload from a file
    bool Load(const std::string& fileName);

    void Unload();

    // Given string and this font, draw to a texture
    class SDL_Texture* RenderText(const std::string& text,
                                  const Vector3& color = Color::White,
                                  int pointSize = 30,
                                  unsigned wrapLength = 1024);

private:
    // Map of point sizes to font data
    std::unordered_map<int, TTF_Font*> mFontData;

    SDL_Renderer* mRenderer;
};