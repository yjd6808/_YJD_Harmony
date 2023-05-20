/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 8:19:21 PM
 * =====================
 *
 */


#pragma once

#include "Tutturu.h"
#include <SteinsGate/Client/Struct.h>

 // TODO: 나중에 Engine으로 빼야함
class SGVec2Ex
{
public:
	static void getLookDirection(const SGVec2& from, const SGVec2& to, JCORE_OUT Direction_t& lr, JCORE_OUT Direction_t& ud);
	static void getLookDirectionX(float from_x, float to_x, JCORE_OUT SpriteDirection_t& lr);
	static void getLookDirectionX(const SGVec2& from, const SGVec2& to, JCORE_OUT SpriteDirection_t& lr);

	static void log(const SGVec2& v) {
		_LogDebug_("%.f, %.f", v.x, v.y);
	}
};


