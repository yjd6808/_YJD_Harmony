/*
 * 작성자: 윤정도
 * 생성일: 1/17/2023 1:21:39 AM
 * =====================
 * 사각형 폴리곤 쉽게 만들기위함.
 */

#pragma once

class RectPoly
{
public:
	RectPoly()
	{
	}

	RectPoly(const RectPoly& _other)
	{
		poly_[0] = _other.poly_[0];
		poly_[1] = _other.poly_[1];
		poly_[2] = _other.poly_[2];
		poly_[3] = _other.poly_[3];
	}

	RectPoly(float _x, float _y, float _width, float _height)
	{
		poly_[0].x = _x;
		poly_[0].y = _y;

		poly_[1].x = _x + _width;
		poly_[1].y = _y;

		poly_[2].x = _x + _width;
		poly_[2].y = _y + _height;

		poly_[3].x = _x;
		poly_[3].y = _y + _height;
	}

	RectPoly(const c2d::vec2& _origin, const c2d::size& _size)
		: RectPoly(_origin.x, _origin.y, _size.width, _size.height)
	{
	}

	c2d::vec2* source()
	{
		return poly_;
	}

	static RectPoly createFromLeftBottom(float _leftBottomX, float _leftBottomY, float _width, float _height)
	{
		return { _leftBottomX, _leftBottomY, _width, _height };
	}

	static RectPoly createFromLeftBottom(const c2d::vec2& _leftBottom, const c2d::size& _size)
	{
		return { _leftBottom.x, _leftBottom.y, _size.width, _size.height };
	}

	static RectPoly createFromRightTop(float _rightTopX, float _rightTopY, float _width, float _height)
	{
		return { _rightTopX - _width, _rightTopY - _height, _width, _height };
	}

	static RectPoly createFromRightTop(const c2d::vec2& _rightTop, const c2d::size& _size)
	{
		return { c2d::vec2{ _rightTop.x - _size.width, _rightTop.y - _size.height }, _size };
	}

	static RectPoly createFromCenter(float _centerX, float _centerY, float _width, float _height)
	{
		return { _centerX - _width / 2, _centerY / _height / 2, _width, _height };
	}

	static RectPoly createFromCenter(const c2d::vec2& _center, const c2d::size& _size)
	{
		return { c2d::vec2{ _center.x - _size.width / 2, _center.y - _size.height / 2 }, _size };
	}

private:
	c2d::vec2 poly_[4];
};
