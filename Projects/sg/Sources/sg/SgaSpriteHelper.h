/*
 * 작성자: 윤정도
 * 생성일: 3/20/2023 7:16:10 PM
 * =====================
 *
 */


#pragma once

#include <sg/SgaSpriteAbstract.h>

struct SgaSpriteHelper
{
public:
	// 전달받은 SgaSpriteAbstractPtr 타입들 중 null인경우 0으로간주해서 가장 큰 너비를 가져온다.
	template <typename... TSprites>
	static float GetMaxWidthF(TSprites&&... _spriteArgs)
	{
		static_assert(sizeof...(_spriteArgs) >= 1, "... need sprite parameter");
		SgaSpriteAbstractPtr pSprites[]{ _spriteArgs... };
		int maxWidth = pSprites[0] != nullptr ? pSprites[0]->GetWidth() : 0;

		for (int spriteIndex = 1; spriteIndex < sizeof...(_spriteArgs); ++spriteIndex)
		{
			if (pSprites[spriteIndex] == nullptr)
				continue;

			const int targetWidth = pSprites[spriteIndex]->GetWidth();
			if (targetWidth > maxWidth)
			{
				maxWidth = targetWidth;
			}
		}

		return maxWidth;
	}

	// 전달받은 SgaSpriteAbstractPtr 타입들 중 null인경우 0으로간주해서 가장 큰 높이를 가져온다.
	template <typename... TSprites>
	static float GetMaxHeightF(TSprites&&... _spriteArgs)
	{
		static_assert(sizeof...(_spriteArgs) >= 1, "... need sprite parameter");
		SgaSpriteAbstractPtr pSprites[]{ _spriteArgs... };
		int maxHeight = pSprites[0] != nullptr ? pSprites[0]->GetHeight() : 0;

		for (int spriteIndex = 1; spriteIndex < sizeof...(_spriteArgs); ++spriteIndex)
		{
			if (pSprites[spriteIndex] == nullptr)
				continue;

			const int targetHeight = pSprites[spriteIndex]->GetHeight();
			if (targetHeight > maxHeight)
			{
				maxHeight = targetHeight;
			}
		}

		return maxHeight;
	}
};
