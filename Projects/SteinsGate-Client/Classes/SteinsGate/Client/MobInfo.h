/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 12:46:28 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/MobBaseInfo.h>
#include <SteinsGate/Client/AnimationInfo.h>
#include <SteinsGate/Client/Struct.h>

struct MobPartInfo
{
	int SgaIndex;
	int ImgIndex;
	int ZOrder;
};


struct MobInfo : MobBaseInfo
{
	MobInfo(int animationSize) : AnimationList(animationSize) {}
	~MobInfo() override = default;

	int PackIndex;
	int SkinImgIndex;
	int PartsIndex[MaxMonsterPartsCount_v];
	int PartsCount;
	
	MobPartInfo Parts[MaxMonsterPartsCount_v];
	SGVector<AnimationInfo> AnimationList;
};
