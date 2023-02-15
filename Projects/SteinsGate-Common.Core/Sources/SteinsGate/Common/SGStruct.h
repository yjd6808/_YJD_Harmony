/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 9:14:24 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/SGEnums.h>

struct SGThicknessBox
{
	float RelativeX;
	float RelativeY;
	float Width;
	float Height;

	SGThicknessBox() : RelativeX(0), RelativeY(0), Width(0), Height(0) {}
	SGThicknessBox(float relaitveX, float relativeY, float width, float height)
		: RelativeX(relaitveX)
		, RelativeY(relativeY)
		, Width(width)
		, Height(height) {}
};




struct SGVisualInfo
{
	int NpkIndex[VisualType::Max];
	int ImgIndex[VisualType::Max];

	SGVisualInfo();
	SGVisualInfo(const SGVisualInfo& other);
};

struct SGCharacterInfo
{
	SGVisualInfo VisualInfo;
};

struct SGActorPartSpriteData
{
	int ZOrder;
	int NpkIndex;
	int ImgIndex;
};


union SGNpkResourceIndex
{
	SGNpkResourceIndex() = default;
	SGNpkResourceIndex(int npkIndex, int imgIndex, int frameIndex)
		: Un{frameIndex, imgIndex, npkIndex}
	{}

	struct
	{
		int FrameIndex  : 12;	// 4096
		int ImgIndex	: 11;	// 2048
		int NpkIndex	: 9;	// 512	
	} Un;

	Int32U Value;
};
