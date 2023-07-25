/*
 * 작성자: 윤정도
 * 생성일: 1/24/2023 9:27:54 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/ConfigDataAbstract.h>
#include <SteinsGate/Client/Struct.h>

struct EffectInfo : ConfigDataAbstract
{
	EffectInfo() = default;
	~EffectInfo() override { JCORE_DELETE_SAFE(SpriteData); }

	SGString Name;
	ActorSpriteData* SpriteData;
};