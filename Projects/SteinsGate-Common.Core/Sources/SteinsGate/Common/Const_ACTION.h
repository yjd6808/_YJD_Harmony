/*
 * 작성자: 윤정도
 * 생성일: 8/26/2023 12:58:37 PM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Common/Namepsace.h>
#include <SteinsGate/Common/Const_FPS.h>

NS_CONST_BEGIN

namespace Action
{
	inline constexpr int ComboSequenceCount = 5;					// 콤보 가능키 최대 몇개?
	inline constexpr float ComboSequenceDelay = FPS::_18;			// 콤보 입력 제한 시간
}


NS_CONST_END