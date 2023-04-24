/*
 * 작성자: 윤정도
 * 생성일: 1/25/2023 11:32:50 AM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Client/AnimationInfo.h>
#include <SteinsGate/Client/Struct.h>

struct ObstacleInfo : ConfigDataAbstract
{
	ObstacleInfo(int animationSize)
		: AnimationList(animationSize) {}
	~ObstacleInfo() override = default;

	SGString Name;
	int SgaIndex;
	int ImgIndex;
	ThicknessBox ThicknessBox;
	bool Hitable;
	bool Colliadalble;
	bool ZOrederable;
	SGVector<AnimationInfo> AnimationList;
};