/*
 * 작성자: 윤정도
 * 생성일: 1/25/2023 11:32:50 AM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Client/SGAnimationInfo.h>
#include <SteinsGate/Client/SGStruct.h>

struct SGObstacleInfo
{
	SGObstacleInfo(int animationSize)
		: AnimationList(animationSize) {}

	int Code;
	SGString Name;
	int NpkIndex;
	int ImgIndex;
	SGThicknessBox ThicknessBox;
	bool Hitable;
	bool Colliadalble;
	bool ZOrederable;
	SGVector<SGAnimationInfo> AnimationList;
};