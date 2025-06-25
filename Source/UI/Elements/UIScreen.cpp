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
}

void UIScreen::Update(float deltaTime)
{
}

void UIScreen::Draw(SDL_Renderer* renderer)
{
    for (UIText* text : mTexts) {
        text->Draw(renderer, mPos);
    }

    for (UIButton* button : mButtons) {
        button->Draw(renderer, mPos);
    }

    for (UIImage* image : mImages) {
        image->Draw(renderer, mPos);
    }
}

void UIScreen::ProcessInput(const uint8_t* keys)
{
}

void UIScreen::HandleKeyPress(int key)
{
    if (mUIType == UIType::HUD) {
        return;
    }

    if (key == SDLK_w) {
        mButtons[mSelectedButtonIndex]->SetHighlighted(false);
        mSelectedButtonIndex--;
        if (mSelectedButtonIndex < 0) {
            mSelectedButtonIndex = static_cast<int>(mButtons.size()) - 1;
        }
        mButtons[mSelectedButtonIndex]->SetHighlighted(true);
    }

    if (key == SDLK_s) {
        mButtons[mSelectedButtonIndex]->SetHighlighted(false);
        mSelectedButtonIndex++;
        if (mSelectedButtonIndex >= static_cast<int>(mButtons.size())) {
            mSelectedButtonIndex = 0;
        }
        mButtons[mSelectedButtonIndex]->SetHighlighted(true);
    }

    if (key == SDLK_RETURN) {
        if (mSelectedButtonIndex >= 0 && mSelectedButtonIndex < static_cast<int>
            (mButtons.size())) {
            mButtons[mSelectedButtonIndex]->OnClick();
        }
    }
}

void UIScreen::Close()
{
    mState = UIState::Closing;
}

UIText* UIScreen::AddText(const std::string& name, const Vector2& pos,
                          const Vector2& dims, const int pointSize,
                          const int unsigned wrapLength)
{
    UIText* t = new UIText(name, mFont, pointSize, wrapLength, pos, dims,
                           Color::White);
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