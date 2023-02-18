/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 12:46:28 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/MobBaseInfo.h>
#include <SteinsGate/Client/SGAnimationInfo.h>
#include <SteinsGate/Client/SGStruct.h>

struct SGMobPartInfo
{
	int PackIndex;
	int ImgIndex;
	int ZOrder;
};


struct SGMobInfo : MobBaseInfo
{
	SGMobInfo(int animationSize) : AnimationList(animationSize) {}
	~SGMobInfo() override = default;

	int PackIndex;
	int SkinImgIndex;
	int PartsIndex[MaxMonsterPartsCount_v];
	int PartsCount;
	
	SGMobPartInfo Parts[MaxMonsterPartsCount_v];
	SGVector<SGAnimationInfo> AnimationList;
};
