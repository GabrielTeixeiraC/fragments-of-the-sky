// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "../../Utils/Math.h"
#include "UIText.h"
#include "UIButton.h"
#include "UIImage.h"
#include "UIRect.h"
#include "UITextButton.h"

class UIScreen
{
public:
    // Tracks if the UI is active or closing
    enum class UIState
    {
        Active,
        Closing
    };

    enum class UIType
    {
        MainMenu,
        PauseMenu,
        FragmentCollected,
        Introduction,
        EndGame,
        HUD
    };

    UIScreen(class Game* game, const std::string& fontName, UIType uiType);

    virtual ~UIScreen();

    // UIScreen subclasses can override these
    virtual void Update(float deltaTime);

    virtual void Draw(class SDL_Renderer* renderer);

    virtual void ProcessInput(const uint8_t* keys);

    virtual void HandleKeyPress(int key);

    // Set state to closing
    void Close();

    // Get state of UI screen
    UIState GetState() const { return mState; }

    // Game getter
    class Game* GetGame() { return mGame; }

    // Add a button to this screen
    UIButton* AddButton(const std::string& name, const Vector2& pos,
                        const Vector2& dims, std::function<void()> onClick,
                        const Vector2& textSize,
                        const std::string& imagePath = "");

    UIText* AddText(const std::string& name, const Vector2& pos,
                    const Vector2& dims, const int pointSize = 40,
                    const int unsigned wrapLength = 1024,
                    Vector3 color = Color::White);

    UIImage* AddImage(const std::string& imagePath, const Vector2& pos,
                      const Vector2& dims, const Vector3& color = Color::White);

    UIRect* AddRect(const Vector2& pos, const Vector2& dims,
                    const Vector3& color = Color::Black, int alpha = 255);

    UITextButton* AddTextButton(const std::string& name, const Vector2& pos,
                                const Vector2& dims,
                                std::function<void()> onClick,
                                const Vector2& textSize,
                                const int pointSize = 40,
                                const int unsigned wrapLength = 1024,
                                Vector3 color = Color::White);

protected:
    // Sets the mouse mode to relative or not
    class Game* mGame;
    class UIFont* mFont;

    // Configure positions
    Vector2 mPos;
    Vector2 mSize;

    // State
    UIState mState;

    // List of buttons, texts, and images
    int mSelectedButtonIndex;
    std::vector<UIButton*> mButtons;
    std::vector<UIText*> mTexts;
    std::vector<UIImage*> mImages;
    std::vector<UIRect*> mRects;
    std::vector<UITextButton*> mTextButtons;

    UIType mUIType;
};