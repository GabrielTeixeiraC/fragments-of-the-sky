#include "HUD.h"
#include "../Game.h"
#include "Elements/UIText.h"

HUD::HUD(class Game* game, const std::string& fontName, UIType uiType)
    : UIScreen(game, fontName, uiType)
{

    // TODO 3.: Adicione um texto com a string "World" à esquerda do texto "Time", como no jogo original.
    mLevelName = AddText("World", Vector2(WORD_OFFSET, HUD_POS_Y),
                         Vector2(CHAR_WIDTH * 3, WORD_HEIGHT), POINT_SIZE);


    // TODO 4.: Adicione um texto com a string "1-1" logo abaixo do texto "World".
    mLevelName = AddText("1-1", Vector2(WORD_OFFSET, HUD_POS_Y + WORD_HEIGHT),
                         Vector2(CHAR_WIDTH * 3, WORD_HEIGHT), POINT_SIZE);

    float iconSize = 32.0f;
    float iconSpacing = 8.0f;
    float iconStartX = WORD_OFFSET;
    float iconY = HUD_POS_Y + WORD_HEIGHT * 4;

    mDoubleJumpIcon = AddImage("../Assets/Sprites/Blocks/BlockA.png",
                               Vector2(iconStartX, iconY),
                               Vector2(iconSize, iconSize));
    mDoubleJumpIcon->SetVisible(false);

    mDashIcon = AddImage("../Assets/Sprites/Blocks/BlockB.png",
                         Vector2(iconStartX + iconSize + iconSpacing, iconY),
                         Vector2(iconSize, iconSize));
    mDashIcon->SetVisible(false);

    mWallJumpIcon = AddImage("../Assets/Sprites/Blocks/BlockC.png",
                             Vector2(iconStartX + 2 * (iconSize + iconSpacing), iconY),
                             Vector2(iconSize, iconSize));
    mWallJumpIcon->SetVisible(false);

}

HUD::~HUD()
{
}

void HUD::SetLevelName(const std::string &levelName)
{
    mLevelName->SetText(levelName);
}

void HUD::onFragmentCollected(Fragment::FragmentType type)
{
    if (type == Fragment::FragmentType::DoubleJump && mDoubleJumpIcon) {
        mDoubleJumpIcon->SetVisible(true);
    }
    else if (type == Fragment::FragmentType::Dash && mDashIcon) {
        mDashIcon->SetVisible(true);
    }
    else if (type == Fragment::FragmentType::WallJump && mWallJumpIcon) {
        mWallJumpIcon->SetVisible(true);
    }
}