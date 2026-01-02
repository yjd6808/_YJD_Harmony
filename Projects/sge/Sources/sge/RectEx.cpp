/*
 * 작성자: 윤정도
 * 생성일: 3/23/2023 3:20:21 PM
 * =====================
 *
 */


#include "Core.h"
#include "RectEx.h"

//////////////////////////////////////////////////////////////////////////////////////////
bool RectEx::Intersect(const cc::rect& _rc, const cc::rect& _rc2, cc::rect& _intersectRect)
{
	if (!_rc.intersectsRect(_rc2))
	{
		return false;
	}

	using namespace jc;

	const float x = Math::Max(_rc.getMinX(), _rc2.getMinX());
	const float y = Math::Max(_rc.getMinY(), _rc2.getMinY());
	const float width = Math::Min(_rc.getMaxX(), _rc2.getMaxX()) - x;
	const float height = Math::Min(_rc.getMaxY(), _rc2.getMaxY()) - y;

	_intersectRect.origin.x = x;
	_intersectRect.origin.y = y;
	_intersectRect.size.width = width;
	_intersectRect.size.height = height;

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void RectEx::Log(const cc::rect& _rc)
{
	jc::Console::WriteLine("::  [%d, %d],  [%d, %d]",
		int(_rc.getMinX()), int(_rc.getMinY()), int(_rc.getWidth()), int(_rc.getHeight()));
}

//////////////////////////////////////////////////////////////////////////////////////////
bool RectEx::IntersectY(const cc::rect& _lhs, const cc::rect& _rhs)
{
	if (_lhs.origin.y > _rhs.origin.y &&
		_lhs.origin.y < _rhs.origin.y + _rhs.size.height)
	{
		return true;
	}

	if (_lhs.origin.y + _lhs.size.height > _rhs.origin.y &&
		_lhs.origin.y + _lhs.size.height < _rhs.origin.y + _rhs.size.height)
	{
		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool RectEx::ContainsX(const cc::rect& _rc, const float _x)
{
	return _x > _rc.origin.x && _x < _rc.origin.x + _rc.size.width;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool RectEx::ContainsY(const cc::rect& _rc, const float _y)
{
	return _y > _rc.origin.y && _y < _rc.origin.y + _rc.size.height;
}
