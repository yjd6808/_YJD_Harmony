/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 8:22:28 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "SGVec2Ex.h"

//////////////////////////////////////////////////////////////////////////////////////////
void SGVec2Ex::getLookDirection(const SGVec2& _from, const SGVec2& _to, JCORE_OUT Direction_t& _lr,
                                JCORE_OUT Direction_t& _ud)
{
	_lr = Direction::Left;
	_ud = Direction::Down;

	if (_to.x - _from.x > 0.0f)
	{
		_lr = Direction::Right;
	}

	if (_to.y - _from.y > 0.0f)
	{
		_ud = Direction::Up;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void SGVec2Ex::getLookDirectionX(float _fromX, float _toX, SpriteDirection_t& _lr)
{
	_lr = SpriteDirection::Left;

	if (_toX - _fromX > 0.0f)
	{
		_lr = SpriteDirection::Right;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void SGVec2Ex::getLookDirectionX(const SGVec2& _from, const SGVec2& _to, SpriteDirection_t& _lr)
{
	_lr = SpriteDirection::Left;

	if (_to.x - _from.x > 0.0f)
	{
		_lr = SpriteDirection::Right;
	}
}
