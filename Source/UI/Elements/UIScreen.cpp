// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "UIScreen.h"
#include "../../Game.h"
#include "UIFont.h"

UIScreen::UIScreen(Game* game, const std::string& fontName, UIType uiType)
    : mGame(game)
      , mPos(0.f, 0.f)
      , mSize(0.f, 0.f)
      , mState(UIState::Active)
      , mSelectedButtonIndex(-1)
      , mUIType(uiType)
{
    mGame->PushUI(this);
    mFont = mGame->LoadFont(fontName);
}

UIScreen::~UIScreen()
{
    for (UIText* text : mTexts) {
        delete text;
    }
    mTexts.clear();

    for (UIButton* button : mButtons) {
        delete button;
    }
    mButtons.clear();

    for (UIImage* image : mImages) {
        delete image;
    }
    mImages.clear();

    for (UIRect* rect : mRects) {
        delete rect;
    }
    mRects.clear();

    for (UITextButton* textButton : mTextButtons) {
        delete textButton;
    }
    mTextButtons.clear();
}

void UIScreen::Update(float deltaTime)
{
}

void UIScreen::Draw(SDL_Renderer* renderer)
{
    for (UIRect* rect : mRects) {
        rect->Draw(renderer, mPos);
    }

    for (UIText* text : mTexts) {
        text->Draw(renderer, mPos);
    }

    for (UIButton* button : mButtons) {
        button->Draw(renderer, mPos);
    }

    for (UIImage* image : mImages) {
        image->Draw(renderer, mPos);
    }

    for (UITextButton* textButton : mTextButtons) {
        textButton->Draw(renderer, mPos);
    }
}

void UIScreen::ProcessInput(const uint8_t* keys)
{
}

void UIScreen::HandleKeyPress(int key)
{
    if (mUIType == UIType::HUD || mUIType == UIType::Introduction || mUIType == UIType::EndGame) return;

    if (key == SDLK_w) {
        if (mUIType == UIType::MainMenu) {
            mButtons[mSelectedButtonIndex]->SetHighlighted(false);
            mSelectedButtonIndex--;
            if (mSelectedButtonIndex < 0) {
                mSelectedButtonIndex = static_cast<int>(mButtons.size()) - 1;
            }
            mButtons[mSelectedButtonIndex]->SetHighlighted(true);

            // Play navigation sound
            if (mGame && mGame->GetAudio()) {
                mGame->GetAudio()->PlaySound("tap.wav");
            }
        } else if (mUIType == UIType::PauseMenu) {
            mTextButtons[mSelectedButtonIndex]->SetHighlighted(false);
            mSelectedButtonIndex--;
            if (mSelectedButtonIndex < 0) {
                mSelectedButtonIndex =
                    static_cast<int>(mTextButtons.size()) - 1;
            }
            mTextButtons[mSelectedButtonIndex]->SetHighlighted(true);

            // Play navigation sound
            if (mGame && mGame->GetAudio()) {
                mGame->GetAudio()->PlaySound("tap.wav");
            }
        }
    }

    if (key == SDLK_s) {
        if (mUIType == UIType::MainMenu) {
            mButtons[mSelectedButtonIndex]->SetHighlighted(false);
            mSelectedButtonIndex++;
            if (mSelectedButtonIndex >= static_cast<int>(mButtons.size())) {
                mSelectedButtonIndex = 0;
            }
            mButtons[mSelectedButtonIndex]->SetHighlighted(true);

            // Play navigation sound
            if (mGame && mGame->GetAudio()) {
                mGame->GetAudio()->PlaySound("tap.wav");
            }
        } else if (mUIType == UIType::PauseMenu) {
            mTextButtons[mSelectedButtonIndex]->SetHighlighted(false);
            mSelectedButtonIndex++;
            if (mSelectedButtonIndex >= static_cast<int>(mTextButtons.size())) {
                mSelectedButtonIndex = 0;
            }
            mTextButtons[mSelectedButtonIndex]->SetHighlighted(true);

            // Play navigation sound
            if (mGame && mGame->GetAudio()) {
                mGame->GetAudio()->PlaySound("tap.wav");
            }
        }
    }

    if (key == SDLK_RETURN) {
        if (mUIType == UIType::MainMenu) {
            if (mSelectedButtonIndex >= 0 && mSelectedButtonIndex < static_cast<
                    int>
                (mButtons.size())) {
                mButtons[mSelectedButtonIndex]->OnClick();
            }
        } else if (mUIType == UIType::PauseMenu) {
            if (mSelectedButtonIndex >= 0 && mSelectedButtonIndex < static_cast<
                    int>
                (mTextButtons.size())) {
                mTextButtons[mSelectedButtonIndex]->OnClick();
            }
        }
    }
}

void UIScreen::Close()
{
    mState = UIState::Closing;
}

UIText* UIScreen::AddText(const std::string& name, const Vector2& pos,
                          const Vector2& dims, const int pointSize,
                          const int unsigned wrapLength, Vector3 color)
{
    UIText* t = new UIText(name, mFont, pointSize, wrapLength, pos, dims,
                           color);
    mTexts.emplace_back(t);
    return t;
}

UIButton* UIScreen::AddButton(const std::string& name, const Vector2& pos,
                              const Vector2& dims,
                              std::function<void()> onClick,
                              const Vector2& textSize,
                              const std::string& imagePath)
{
    UIButton* b = new UIButton(name, mFont, onClick, pos, dims, Color::Orange,
                               16, 1024, Vector2::Zero, textSize, Color::White,
                               mGame->GetRenderer(), imagePath);
    mButtons.emplace_back(b);

    if (mButtons.size() == 1) {
        mSelectedButtonIndex = 0;
        b->SetHighlighted(true);
    }

    return b;
}

UIImage* UIScreen::AddImage(const std::string& imagePath, const Vector2& pos,
                            const Vector2& dims, const Vector3& color)
{
    UIImage* img = new UIImage(mGame->GetRenderer(), imagePath, pos, dims,
                               color);
    mImages.emplace_back(img);
    return img;
}

UIRect* UIScreen::AddRect(const Vector2& pos, const Vector2& dims,
                          const Vector3& color, int alpha)
{
    UIRect* rect = new UIRect(pos, dims, color, alpha, mGame->GetRenderer());
    mRects.emplace_back(rect);
    return rect;
}

UITextButton* UIScreen::AddTextButton(const std::string& name,
                                      const Vector2& pos,
                                      const Vector2& dims,
                                      std::function<void()> onClick,
                                      const Vector2& textSize,
                                      const int pointSize,
                                      const int unsigned wrapLength,
                                      Vector3 color)
{
    UITextButton* b = new UITextButton(name, mFont, onClick, pos, dims,
                                       Color::LightBlue,
                                       pointSize, wrapLength, Vector2::Zero,
                                       textSize,
                                       color,
                                       mGame->GetRenderer());
    mTextButtons.emplace_back(b);
    if (mTextButtons.size() == 1) {
        mSelectedButtonIndex = 0;
        b->SetHighlighted(true);
    }
    return b;
}