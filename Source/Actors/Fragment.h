#pragma once

#include "Actor.h"

class Fragment : public Actor
{
public:
    enum class FragmentType { DoubleJump, Dash, WallJump };

    Fragment(class Game* game, FragmentType type);

    FragmentType GetType() const { return mFragmentType; }

    void OnCollected();

private:
    FragmentType mFragmentType;
};