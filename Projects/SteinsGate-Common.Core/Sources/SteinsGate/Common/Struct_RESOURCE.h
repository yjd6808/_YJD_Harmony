/*
 * 작성자: 윤정도
 * 생성일: 8/28/2023 10:59:06 AM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Common/Enum.h>
#include <SteinsGate/Common/Const.h>
#include <SteinsGate/Common/Type.h>

union SgaResourceIndex
{
	SgaResourceIndex();
	SgaResourceIndex(int sgaIndex, int imgIndex, int frameIndex)
		: Un{ sgaIndex, imgIndex, frameIndex }
	{}

	struct
	{
		int SgaIndex : 9;	// 512
		int ImgIndex : 11;	// 2048
		int FrameIndex : 12;	// 4096

	} Un;

	SGString ToString() const;
	Int32U Value;
};