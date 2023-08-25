/*
 * 작성자: 윤정도
 * 생성일: 8/25/2023 7:55:14 PM
 * =====================
 *
 */


#pragma once


#include <JCore/Define.h>



JCORE_SENUM_BEGIN(HAlignment)
Left,
Center,
Right
JCORE_SENUM_END(HAlignment)

// ==========================================================================================

JCORE_SENUM_BEGIN(VAlignment)
Top,
Center,
Bottom
JCORE_SENUM_END(VAlignment)

// ==========================================================================================

JCORE_SENUM_BEGIN(UIElementType)
Group,
Button,
Label,
Sprite,
EditBox,
CheckBox,
ToggleButton,
ScrollBar,
ProgressBar,
Static,
Max

JCORE_SENUM_MIDDLE(UIElementType)

static constexpr const char* Name[Max]{
	"그룹",
	"버튼",
	"라벨",
	"스프라이트",
	"에딧박스",
	"체크박스",
	"토글버튼",
	"스크롤바",
	"프로그래스바",
	"스태틱"
};


JCORE_SENUM_MIDDLE_END(UIElementType)

// ==========================================================================================

JCORE_SENUM_BEGIN(ProgressIncreaseDirection)
LeftRight,
RightLeft,
TopBottom,
BottomTop
JCORE_SENUM_END(ProgressIncreaseDirection)

// ==========================================================================================