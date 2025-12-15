/*
 * : 
 * : 3/23/2023 3:20:21 PM
 * =====================
 *
 */


#include "Engine.h"
#include "RectEx.h"

//////////////////////////////////////////////////////////////////////////////////////////
bool RectEx::Intersect(const SGRect& _rc, const SGRect& _rc2, SGRect& _intersectRect)
{
	if (!_rc.intersectsRect(_rc2))
	{
		return false;
	}

	using namespace JCore;

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
void RectEx::Log(const SGRect& _rc)
{
	JCore::Console::WriteLine("::  [%d, %d],  [%d, %d]",
		int(_rc.getMinX()), int(_rc.getMinY()), int(_rc.getWidth()), int(_rc.getHeight()));
}

//////////////////////////////////////////////////////////////////////////////////////////
bool RectEx::IntersectY(const SGRect& _lhs, const SGRect& _rhs)
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
bool RectEx::ContainsX(const SGRect& _rc, const float _x)
{
	return _x > _rc.origin.x && _x < _rc.origin.x + _rc.size.width;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool RectEx::ContainsY(const SGRect& _rc, const float _y)
{
	return _y > _rc.origin.y && _y < _rc.origin.y + _rc.size.height;
}
