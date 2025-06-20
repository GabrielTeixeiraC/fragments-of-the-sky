#include "HUD.h"
#include "../Game.h"
#include "Elements/UIText.h"

HUD::HUD(class Game* game, const std::string& fontName, UIType uiType)
    : UIScreen(game, fontName, uiType)
{
    // Create HUD elements here
}

HUD::~HUD()
{
}