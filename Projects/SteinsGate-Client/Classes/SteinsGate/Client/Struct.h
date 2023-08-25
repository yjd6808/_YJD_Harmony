/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 9:14:24 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/Enum.h>
#include <SteinsGate/Common/Struct.h>

#include <SteinsGate/Client/Const.h>

struct AnimationInfo;
struct AttackDataInfo;

class Actor;

struct ActorRect
{
	SGRect ThicknessRect;
	SGRect BodyRect;

	bool isCollide(const ActorRect& rect, SpriteDirection_t& otherHitDirection, SGRect& hitRect);
};

struct HitInfo
{
	Actor* Attacker;
	Actor* HitTarget;
	SpriteDirection_t HitDirection;
	SGRect HitRect;
	AttackDataInfo* AttackDataInfo;
};

struct ChannelDensityEx : ChannelDensity
{
	inline const static SGColor3B Color[Max]{
		SGColor3B{ 34, 139, 34 },	// 쾌적
		SGColor3B{ 237, 145, 33 },	// 보통
		SGColor3B{ 178, 34, 34 },	// 혼잡
		SGColor3B{ 255, 64, 169 }	// 포화
	};
};