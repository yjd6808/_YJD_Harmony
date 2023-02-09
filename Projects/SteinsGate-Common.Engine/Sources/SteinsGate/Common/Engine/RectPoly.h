/*
 * 작성자: 윤정도
 * 생성일: 1/17/2023 1:21:39 AM
 * =====================
 * 사각형 폴리곤 쉽게 만들기위함.
 */


#pragma once



#include <SteinsGate/Common/Engine.h>

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

	RectPoly(const cocos2d::Vec2& origin, const cocos2d::Size& size) {
		poly[0].x = origin.x;
		poly[0].y = origin.y;

		poly[1].x = origin.x + size.width;
		poly[1].y = origin.y;

		poly[2].x = origin.x + size.width;
		poly[2].y = origin.y + size.height;

		poly[3].x = origin.x;
		poly[3].y = origin.y + size.height;
	}

	cocos2d::Vec2* source() { return poly; }

	static RectPoly createFromLeftBottom(const cocos2d::Vec2& leftBottom, const cocos2d::Size& size) {
		return { leftBottom, size};
	}

	static RectPoly createFromRightTop(const cocos2d::Vec2& rightTop, const cocos2d::Size& size) {
		return { cocos2d::Vec2{rightTop.x - size.width, rightTop.y - size.height }, size };
	}

	static RectPoly createFromCenter(const cocos2d::Vec2& center, const cocos2d::Size& size) {
		return { cocos2d::Vec2{ center.x - size.width / 2, center.y - size.height / 2 }, size };
	}
private:
	cocos2d::Vec2 poly[4];
};