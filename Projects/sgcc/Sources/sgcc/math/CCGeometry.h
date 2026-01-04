/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2013-2017 Chukong Technologies
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#ifndef __MATH_CCGEOMETRY_H__
#define __MATH_CCGEOMETRY_H__

#include "sgcc/math/CCMath.h"

/**
 * @addtogroup base
 * @{
 */

NS_CC_BEGIN

class SGCC_DLL Size
{
public:
    /**Width of the Size.*/
    float width;
    /**Height of the Size.*/
    float height;
public:
    /**Conversion from Vec2 to Size.*/
    operator Vec2() const
    {
        return Vec2(width, height);
    }

public:
    /**
    @{
    Constructor.
    @param width Width of the size.
    @param height Height of the size.
    @param other Copy constructor.
    @param point Conversion from a point.
     */
    Size();
    Size(float width, float height);
    Size(const Size& other);
    explicit Size(const Vec2& point);
    /**@}*/

    /**
     * @js NA
     * @lua NA
     */
    Size& operator= (const Size& other);
    /**
     * @js NA
     * @lua NA
     */
    Size& operator= (const Vec2& point);
    /**
     * @js NA
     * @lua NA
     */
    Size operator+(const Size& right) const;
    /**
     * @js NA
     * @lua NA
     */
    Size operator-(const Size& right) const;
    /**
     * @js NA
     * @lua NA
     */
    Size operator*(float a) const;
    /**
     * @js NA
     * @lua NA
     */
    Size operator/(float a) const;
    /**
    Set the width and height of Size.
     * @js NA
     * @lua NA
     */
    void setSize(float width, float height);
    /**
    Check if two size is the same.
     * @js NA
     */
    bool equals(const Size& target) const;
    /**Size(0,0).*/
    static const Size ZERO;
};

/**Rectangle area.*/
class SGCC_DLL Rect
{
public:
    /**Low left point of rect.*/
    Vec2 origin;
    /**Width and height of the rect.*/
    Size  size;

public:
    /**
    Constructor an empty Rect.
     * @js NA
     */
    Rect();
    /**
    Constructor a rect.
     * @js NA
     */
    Rect(float x, float y, float width, float height);
    /**
     Constructor a rect.
     * @js NA
     */
    Rect(const Vec2& pos, const Size& dimension);
    /**
    Copy constructor.
     * @js NA
     * @lua NA
     */
    Rect(const Rect& other);
    /**
     * @js NA
     * @lua NA
     */
    Rect& operator= (const Rect& other);
    /**
    Set the x, y, width and height of Rect.
     * @js NA
     * @lua NA
     */
    void setRect(float x, float y, float width, float height);
    /**
    Get the left of the rect.
     * @js NA
     */
    float getMinX() const; /// return the leftmost x-value of current rect
    /**
    Get the X coordinate of center point.
     * @js NA
     */
    float getMidX() const; /// return the midpoint x-value of current rect
    /**
    Get the right of rect.
     * @js NA
     */
    float getMaxX() const; /// return the rightmost x-value of current rect
    /**
    Get the bottom of rect.
     * @js NA
     */
    float getMinY() const; /// return the bottommost y-value of current rect
    /**
    Get the Y coordinate of center point.
     * @js NA
     */
    float getMidY() const; /// return the midpoint y-value of current rect
    /**
    Get top of rect.
     * @js NA
     */
    float getMaxY() const; /// return the topmost y-value of current rect

    // 부족한 기능 추가
    Vec2 getRightTop() const { return { origin.x + size.width, origin.y + size.height }; }
    Vec2 getLeftBottom() const { return origin; }
    Vec2 getMid() const { return { origin.x + size.width / 2, origin.y + size.height / 2 }; }
    float getOriginX() const { return origin.x; }
    float getOriginY() const { return origin.y; }
    float getWidth() const { return size.width; }
    float getHeight() const { return size.height; }
    Size getSize() const { return size; }



    /**
    Compare two rects.
     * @js NA
     */
    bool equals(const Rect& rect) const;
    /**
    Check if the points is contained inclusively in the rect.
     * @js NA
     */
    bool containsPoint(const Vec2& point) const;
    /**
    Check the intersect status of two rects.
     * @js NA
     */
    bool intersectsRect(const Rect& rect) const;
    /**
    Check the intersect status of the rect and a circle.
     * @js NA
     */
    bool intersectsCircle(const Vec2& center, float radius) const;
    /**
    Get the min rect which can contain this and rect.
     * @js NA
     * @lua NA
     */
    Rect unionWithRect(const Rect & rect) const;
    /**Compute the min rect which can contain this and rect, assign it to this.*/
    void merge(const Rect& rect);
    /**An empty Rect.*/
    static const Rect ZERO;

	/*
	 * \brief 인터섹트 결과와 더불어 겹쳐진 영역의 위치를 반환하도록 한다.
	 * 하단 테스트코드 참고
	 */
	static bool Intersect(const Rect& _rc, const Rect& _rc2, Rect& _intersectRect);
	std::string ToString() const;

	/*
	{
		Rect r1{ 200, 200, 100, 100 };
		Rect r2{ 250, 250, 100, 100 };
		Rect r3;
		if (rect::intersect(r1, r2, r3)) {
			Log("%d, %d\n", int(r3.getMinX()), int(r3.getMinY()));
			Log("%d, %d\n", int(r3.getWidth()), int(r3.getHeight()));
		}
	}

	{
		Rect r1{ 200, 200, 100, 100 };
		Rect r2{ 250, 150, 100, 100 };
		Rect r3;
		if (rect::intersect(r1, r2, r3)) {
			Log("%d, %d\n", int(r3.getMinX()), int(r3.getMinY()));
			Log("%d, %d\n", int(r3.getWidth()), int(r3.getHeight()));
		}
	}


	{
		Rect r1{ 200, 200, 100, 100 };
		Rect r2{ 150, 150, 100, 100 };
		Rect r3;
		if (rect::intersect(r1, r2, r3)) {
			Log("%d, %d\n", int(r3.getMinX()), int(r3.getMinY()));
			Log("%d, %d\n", int(r3.getWidth()), int(r3.getHeight()));
		}
	}
	{
		Rect r1{ 200, 200, 100, 100 };
		Rect r2{ 150, 250, 100, 100 };
		Rect r3;
		if (rect::intersect(r1, r2, r3)) {
			Log("%d, %d\n", int(r3.getMinX()), int(r3.getMinY()));
			Log("%d, %d\n", int(r3.getWidth()), int(r3.getHeight()));
		}
	}

	{
		Rect r2{ 200, 200, 100, 100 };
		Rect r1{ 250, 250, 100, 100 };
		Rect r3;
		if (rect::intersect(r1, r2, r3)) {
			Log("%d, %d\n", int(r3.getMinX()), int(r3.getMinY()));
			Log("%d, %d\n", int(r3.getWidth()), int(r3.getHeight()));
		}
	}

	{
		Rect r2{ 200, 200, 100, 100 };
		Rect r1{ 250, 150, 100, 100 };
		Rect r3;
		if (rect::intersect(r1, r2, r3)) {
			Log("%d, %d\n", int(r3.getMinX()), int(r3.getMinY()));
			Log("%d, %d\n", int(r3.getWidth()), int(r3.getHeight()));
		}
	}


	{
		Rect r2{ 200, 200, 100, 100 };
		Rect r1{ 150, 150, 100, 100 };
		Rect r3;
		if (rect::intersect(r1, r2, r3)) {
			Log("%d, %d\n", int(r3.getMinX()), int(r3.getMinY()));
			Log("%d, %d\n", int(r3.getWidth()), int(r3.getHeight()));
		}
	}
	{
		Rect r2{ 200, 200, 100, 100 };
		Rect r1{ 150, 250, 100, 100 };
		Rect r3;
		if (rect::intersect(r1, r2, r3)) {
			Log("%d, %d\n", int(r3.getMinX()), int(r3.getMinY()));
			Log("%d, %d\n", int(r3.getWidth()), int(r3.getHeight()));
		}
	}

	250, 250
	50, 50
	250, 200
	50, 50
	200, 200
	50, 50
	200, 250
	50, 50

	250, 250
	50, 50
	250, 200
	50, 50
	200, 200
	50, 50
	200, 250
	50, 50

	*/


	// lhs와 rhs의 세로길이가 겹치는지
	static bool IntersectY(const Rect& _lhs, const Rect& _rhs);

	// rc의 가로길이와 pos의 x좌표가 겹치는지
	static bool ContainsX(const Rect& _rc, const float _x);

	// rc의 세로길이와 pos의 y좌표가 겹치는지
	static bool ContainsY(const Rect& _rc, const float _y);
};

class SGCC_DLL RectPoly
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

	RectPoly(const Vec2& _origin, const Size& _size) : RectPoly(_origin.x, _origin.y, _size.width, _size.height)
	{
	}

	Vec2* source()
	{
		return poly_;
	}

	static RectPoly createFromLeftBottom(float _leftBottomX, float _leftBottomY, float _width, float _height)
	{
		return { _leftBottomX, _leftBottomY, _width, _height };
	}

	static RectPoly createFromLeftBottom(const Vec2& _leftBottom, const Size& _size)
	{
		return { _leftBottom.x, _leftBottom.y, _size.width, _size.height };
	}

	static RectPoly createFromRightTop(float _rightTopX, float _rightTopY, float _width, float _height)
	{
		return { _rightTopX - _width, _rightTopY - _height, _width, _height };
	}

	static RectPoly createFromRightTop(const Vec2& _rightTop, const Size& _size)
	{
		return { Vec2{ _rightTop.x - _size.width, _rightTop.y - _size.height }, _size };
	}

	static RectPoly createFromCenter(float _centerX, float _centerY, float _width, float _height)
	{
		return { _centerX - _width / 2, _centerY / _height / 2, _width, _height };
	}

	static RectPoly createFromCenter(const Vec2& _center, const Size& _size)
	{
		return { Vec2{ _center.x - _size.width / 2, _center.y - _size.height / 2 }, _size };
	}

private:
	Vec2 poly_[4];
};

NS_CC_END

// end of base group
/// @}

#endif // __MATH_CCGEOMETRY_H__
