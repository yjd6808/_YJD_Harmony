/*
 * 작성자: 윤정도
 * 생성일: 3/20/2023 7:16:10 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/SgaSpriteAbstract.h>

struct SgaSpriteHelper
{
public:

    // 전달받은 SgaSpriteAbstractPtr 타입들 중 null인경우 0으로간주해서 가장 큰 너비를 가져온다.
	template <typename... TSprites>
	static float GetMaxWidthF(TSprites&&... spriteArgs) {

        static_assert(sizeof...(spriteArgs) >= 1, "... need sprite parameter");
        SgaSpriteAbstractPtr sprites[] { spriteArgs... };
        int iMaxWidth = sprites[0] != nullptr ? sprites[0]->GetWidth() : 0;

        for (int i = 1; i < sizeof...(spriteArgs); ++i) {
            if (sprites[i] == nullptr)
                continue;

            const int iTargetWidth = sprites[i]->GetWidth();
            if (iTargetWidth > iMaxWidth) {
                iMaxWidth = iTargetWidth;
            }
        }

        return iMaxWidth;
	}

    // 전달받은 SgaSpriteAbstractPtr 타입들 중 null인경우 0으로간주해서 가장 큰 높이를 가져온다.
    template <typename... TSprites>
	static float GetMaxHeightF(TSprites&&... spriteArgs) {
        static_assert(sizeof...(spriteArgs) >= 1, "... need sprite parameter");
        SgaSpriteAbstractPtr sprites[] { spriteArgs... };
        int iMaxWidth = sprites[0] != nullptr ? sprites[0]->GetWidth() : 0;

        for (int i = 1; i < sizeof...(spriteArgs); ++i) {
            if (sprites[i] == nullptr)
                continue;

            const int iTargetWidth = sprites[i]->GetWidth();
            if (iTargetWidth > iMaxWidth) {
                iMaxWidth = iTargetWidth;
            }
        }

        return iMaxWidth;
	}
};