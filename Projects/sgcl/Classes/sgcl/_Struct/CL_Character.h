#pragma once

#include <sg/_Struct/SteinsGate_CharBase.h>

class Actor;
struct AttackDataInfo;

struct HitInfo
{
    Actor* pAttacker_;
    Actor* pHitTarget_;
    SpriteDirection_t hitDirection_;
    cc::rect hitRect_;
    AttackDataInfo* pAttackDataInfo_;
};