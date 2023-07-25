/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 12:46:28 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/MobBaseInfo.h>
#include <SteinsGate/Client/Struct.h>

struct MobInfo : MobBaseInfo
{
	MobInfo() : SpriteData(nullptr) {}
	~MobInfo() override { JCORE_DELETE_SAFE(SpriteData); }

	ActorSpriteData* SpriteData;
	/*int PackIndex;
	int SkinImgIndex;
	int PartsIndex[MaxMonsterPartsCount_v];
	int PartsCount;
	
	ActorPartSpriteData Parts[MaxMonsterPartsCount_v];
	SGVector<AnimationInfo> AnimationList;*/
};
