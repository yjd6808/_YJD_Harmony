/*
 * 작성자 : 윤정도
 */

#pragma once

#include <TF/SourceHeader.h>

struct NodeUtil
{
	static bool IsCollideCircle(const Vec2& p1, const float r1, const Vec2 p2, const float r2) {
		return p1.distance(p2) < (r1 + r2);
		
	}
};
