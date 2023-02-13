/*
 * 작성자: 윤정도
 * 생성일: 1/17/2023 1:21:39 AM
 * =====================
 * 사각형 폴리곤 쉽게 만들기위함.
 */


#pragma once



#include "Engine.h"

class RectPoly
{
public:
	RectPoly() {}	// 전부 0
	RectPoly(const RectPoly& other) {
		poly[0] = other.poly[0];
		poly[1] = other.poly[1];
		poly[2] = other.poly[2];
		poly[3] = other.poly[3];
	}

	RectPoly(float x, float y, float width, float height) {
		poly[0].x = x;
		poly[0].y = y;

		poly[1].x = x + width;
		poly[1].y = y;

		poly[2].x = x + width;
		poly[2].y = y + height;

		poly[3].x = x;
		poly[3].y = y + height;
	}

	RectPoly(const SGVec2& origin, const SGSize& size)
		: RectPoly(origin.x, origin.y, size.width, size.height)
	{}

	SGVec2* source() { return poly; }

	static RectPoly createFromLeftBottom(float leftBottom_x, float leftBottom_y, float width, float height) {
		return { leftBottom_x, leftBottom_y, width, height };
	}

	static RectPoly createFromLeftBottom(const SGVec2& leftBottom, const SGSize& size) {
		return { leftBottom.x, leftBottom.y, size.width, size.height };
	}

	static RectPoly createFromRightTop(float rightTop_x, float rightTop_y, float width, float height) {
		return { rightTop_x - width, rightTop_y - height, width, height };
	}

	static RectPoly createFromRightTop(const SGVec2& rightTop, const SGSize& size) {
		return { SGVec2{rightTop.x - size.width, rightTop.y - size.height }, size };
	}

	static RectPoly createFromCenter(float center_x, float center_y, float width, float height) {
		return { center_x - width / 2, center_y / height / 2, width, height };
	}

	static RectPoly createFromCenter(const SGVec2& center, const SGSize& size) {
		return { SGVec2{ center.x - size.width / 2, center.y - size.height / 2 }, size };
	}
private:
	SGVec2 poly[4];
};