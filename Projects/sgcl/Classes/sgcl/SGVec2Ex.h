/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 8:19:21 PM
 * =====================
 *
 */


#pragma once

#include <sgcl/Core.h>
#include <sgcl/Struct.h>

// TODO: 나중에 Engine으로 빼야함
class SGVec2Ex
{
public:
	static void GetLookDirection(const cc::vec2& _from, const cc::vec2& _to, OUT Direction_t& _lr, OUT Direction_t& _ud);
	static void GetLookDirectionX(float _fromX, float _toX, OUT SpriteDirection_t& _lr);
	static void GetLookDirectionX(const cc::vec2& _from, const cc::vec2& _to, OUT SpriteDirection_t& _lr);

	static void Log(const cc::vec2& _value)
	{
		_LogDebug_("%.f, %.f", _value.x, _value.y);
	}
};
