/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 8:19:21 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/Tutturu.h>
#include <SteinsGate/Client/Struct.h>

// TODO: 나중에 Engine으로 빼야함
class SGVec2Ex
{
public:
	static void getLookDirection(const SGVec2& _from, const SGVec2& _to, JCORE_OUT Direction_t& _lr,
	                             JCORE_OUT Direction_t& _ud);
	static void getLookDirectionX(float _fromX, float _toX, JCORE_OUT SpriteDirection_t& _lr);
	static void getLookDirectionX(const SGVec2& _from, const SGVec2& _to, JCORE_OUT SpriteDirection_t& _lr);

	static void log(const SGVec2& _value)
	{
		_LogDebug_("%.f, %.f", _value.x, _value.y);
	}
};
