/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 9:14:24 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Research/Config.h>

struct SGThicknessBox
{
	float Width;
	float Height;
	float RelativeY;

	cocos2d::Size getSize() const { return { Width, Height }; }
};


struct SGVisualInfo
{
	int NpkIndex[VisualType::Max];
	int ImgIndex[VisualType::Max];

	SGVisualInfo() {
		JCore::Arrays::Fill(NpkIndex, InvalidIndex_v);
		JCore::Arrays::Fill(ImgIndex, InvalidIndex_v);
	}
	SGVisualInfo(const SGVisualInfo& other) {
		JCore::Memory::CopyUnsafe(NpkIndex, other.NpkIndex, sizeof(int) * VisualType::Max);
		JCore::Memory::CopyUnsafe(ImgIndex, other.ImgIndex, sizeof(int) * VisualType::Max);
	}
};

struct SGCharacterInfo
{
	SGVisualInfo VisualInfo;
	SGVector<int> ValidAction;
};

struct SGActorPartSpriteData
{
	int ZOrder;
	int NpkIndex;
	int ImgIndex;
};


struct SGAnimationInfo;
struct SGActorSpriteData
{
	SGVector<SGActorPartSpriteData> Parts;
	SGVector<SGAnimationInfo*> Animations;
};

using SGActorSpriteDataPtr = JCore::SharedPtr<SGActorSpriteData>;