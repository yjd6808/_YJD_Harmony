/*
 * 작성자: 윤정도
 * 생성일: 1/16/2023 3:00:20 AM
 * =====================
 *
 */


#pragma once

#include "Core.h"

class RectEx : c2d::rect
{
public:
	/**
	 * \brief 인터섹트 결과와 더불어 겹쳐진 영역의 위치를 반환하도록 한다.
	 * 하단 테스트코드 참고
	 */
	static bool Intersect(const c2d::rect& _rc, const c2d::rect& _rc2, c2d::rect& _intersectRect);
	static void Log(const c2d::rect& _rc);

	/*
	{
		Rect r1{ 200, 200, 100, 100 };
		Rect r2{ 250, 250, 100, 100 };
		Rect r3;
		if (RectEx::intersect(r1, r2, r3)) {
			Log("%d, %d\n", int(r3.getMinX()), int(r3.getMinY()));
			Log("%d, %d\n", int(r3.getWidth()), int(r3.getHeight()));
		}
	}

	{
		Rect r1{ 200, 200, 100, 100 };
		Rect r2{ 250, 150, 100, 100 };
		Rect r3;
		if (RectEx::intersect(r1, r2, r3)) {
			Log("%d, %d\n", int(r3.getMinX()), int(r3.getMinY()));
			Log("%d, %d\n", int(r3.getWidth()), int(r3.getHeight()));
		}
	}


	{
		Rect r1{ 200, 200, 100, 100 };
		Rect r2{ 150, 150, 100, 100 };
		Rect r3;
		if (RectEx::intersect(r1, r2, r3)) {
			Log("%d, %d\n", int(r3.getMinX()), int(r3.getMinY()));
			Log("%d, %d\n", int(r3.getWidth()), int(r3.getHeight()));
		}
	}
	{
		Rect r1{ 200, 200, 100, 100 };
		Rect r2{ 150, 250, 100, 100 };
		Rect r3;
		if (RectEx::intersect(r1, r2, r3)) {
			Log("%d, %d\n", int(r3.getMinX()), int(r3.getMinY()));
			Log("%d, %d\n", int(r3.getWidth()), int(r3.getHeight()));
		}
	}

	{
		Rect r2{ 200, 200, 100, 100 };
		Rect r1{ 250, 250, 100, 100 };
		Rect r3;
		if (RectEx::intersect(r1, r2, r3)) {
			Log("%d, %d\n", int(r3.getMinX()), int(r3.getMinY()));
			Log("%d, %d\n", int(r3.getWidth()), int(r3.getHeight()));
		}
	}

	{
		Rect r2{ 200, 200, 100, 100 };
		Rect r1{ 250, 150, 100, 100 };
		Rect r3;
		if (RectEx::intersect(r1, r2, r3)) {
			Log("%d, %d\n", int(r3.getMinX()), int(r3.getMinY()));
			Log("%d, %d\n", int(r3.getWidth()), int(r3.getHeight()));
		}
	}


	{
		Rect r2{ 200, 200, 100, 100 };
		Rect r1{ 150, 150, 100, 100 };
		Rect r3;
		if (RectEx::intersect(r1, r2, r3)) {
			Log("%d, %d\n", int(r3.getMinX()), int(r3.getMinY()));
			Log("%d, %d\n", int(r3.getWidth()), int(r3.getHeight()));
		}
	}
	{
		Rect r2{ 200, 200, 100, 100 };
		Rect r1{ 150, 250, 100, 100 };
		Rect r3;
		if (RectEx::intersect(r1, r2, r3)) {
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
	static bool IntersectY(const c2d::rect& _lhs, const c2d::rect& _rhs);

	// rc의 가로길이와 pos의 x좌표가 겹치는지
	static bool ContainsX(const c2d::rect& _rc, const float _x);

	// rc의 세로길이와 pos의 y좌표가 겹치는지
	static bool ContainsY(const c2d::rect& _rc, const float _y);

};