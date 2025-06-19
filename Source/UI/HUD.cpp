#include "HUD.h"
#include "../Game.h"
#include "Elements/UIText.h"

HUD::HUD(class Game* game, const std::string& fontName, UIType uiType)
    : UIScreen(game, fontName, uiType)
{
    // AddText(
    //     "Time", Vector2(mGame->GetWindowWidth() - HUD::CHAR_WIDTH * 4,
    //                     HUD::HUD_POS_Y),
    //     Vector2(HUD::CHAR_WIDTH * 4, HUD::WORD_HEIGHT), 18);
    //
    mTimeText = AddText(
        "400", Vector2(mGame->GetWindowWidth() - HUD::CHAR_WIDTH * 3.1,
                       HUD::HUD_POS_Y + 25.0f),
        Vector2(HUD::CHAR_WIDTH * 3, HUD::WORD_HEIGHT), 18);
    //
    // AddText("World",
    //         Vector2(mGame->GetWindowWidth() / 1.5f - HUD::CHAR_WIDTH * 3,
    //                 HUD::HUD_POS_Y),
    //         Vector2(HUD::CHAR_WIDTH * 3, HUD::WORD_HEIGHT), 18);
    //
    mLevelName = AddText(
        "1-1", Vector2(mGame->GetWindowWidth() / 1.5f - HUD::CHAR_WIDTH * 2.1,
                       HUD::HUD_POS_Y + 25.0f),
        Vector2(HUD::CHAR_WIDTH * 3, HUD::WORD_HEIGHT), 18);
    //
    // AddImage("../Assets/Sprites/Collectables/Coin.png",
    //          Vector2(mGame->GetWindowWidth() / 3.0f, HUD::HUD_POS_Y + 10.0f),
    //          Vector2(WORD_HEIGHT, WORD_HEIGHT));
    // AddText(
    //     "x", Vector2(mGame->GetWindowWidth() / 3.0f + CHAR_WIDTH,
    //                  HUD::HUD_POS_Y + 10.0f),
    //     Vector2(HUD::CHAR_WIDTH * 3, HUD::WORD_HEIGHT), 18);
    mCoinsCounter = AddText(
        "00", Vector2(mGame->GetWindowWidth() / 3.0f + CHAR_WIDTH * 2,
                      HUD::HUD_POS_Y + 10.0f),
        Vector2(HUD::CHAR_WIDTH * 3, HUD::WORD_HEIGHT), 18);
    //
    // AddText("Mario", Vector2(HUD::HUD_POS_Y, HUD::HUD_POS_Y),
    //         Vector2(300.0f, 30.0f), 18);
    //
    mScoreCounter = AddText("000000",
                            Vector2(HUD::HUD_POS_Y, HUD::HUD_POS_Y + 25.0f),
                            Vector2(HUD::CHAR_WIDTH * 6, HUD::WORD_HEIGHT), 18);
}

HUD::~HUD()
{
}

void HUD::SetTime(int time)
{
    mTimeText->SetText(std::to_string(time));

    if (time < 10) {
        mTimeText->SetPosition(Vector2(
            mGame->GetWindowWidth() - CHAR_WIDTH * 1.3, HUD_POS_Y + 25.0f));
    } else if (time < 100) {
        mTimeText->SetPosition(Vector2(
            mGame->GetWindowWidth() - CHAR_WIDTH * 2.18, HUD_POS_Y + 25.0f));
    }
}

void HUD::SetCoins(int coins)
{
    mCoinsCounter->SetText(coins <= 9
                               ? "0" + std::to_string(coins)
                               : std::to_string(Math::Clamp(coins, 0, 99)));
}


void HUD::SetLevelName(const std::string& levelName)
{
    mLevelName->SetText(levelName);
}