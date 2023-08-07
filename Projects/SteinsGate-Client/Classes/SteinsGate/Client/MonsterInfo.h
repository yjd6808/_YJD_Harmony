/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 12:46:28 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/MonsterBaseInfo.h>
#include <SteinsGate/Client/Struct.h>

struct MonsterInfo : MonsterBaseInfo
{
	MonsterInfo() : SpriteData(nullptr) {}
	~MonsterInfo() override { JCORE_DELETE_SAFE(SpriteData); }

	ActorSpriteData* SpriteData;
};
