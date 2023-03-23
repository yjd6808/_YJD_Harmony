/*
 * 작성자: 윤정도
 * 생성일: 3/23/2023 3:20:21 PM
 * =====================
 *
 */


#include "Engine.h"
#include "SGRectEx.h"

bool SGRectEx::intersect(const SGRect& rc, const SGRect& rc2, SGRect& intersectRect) {
	if (!rc.intersectsRect(rc2)) {
		return false;
	}

	using namespace JCore;

	const float x = Math::Max(rc.getMinX(), rc2.getMinX());
	const float y = Math::Max(rc.getMinY(), rc2.getMinY());
	const float width = Math::Min(rc.getMaxX(), rc2.getMaxX()) - x;
	const float height = Math::Min(rc.getMaxY(), rc2.getMaxY()) - y;

	intersectRect.origin.x = x;
	intersectRect.origin.y = y;
	intersectRect.size.width = width;
	intersectRect.size.height = height;
	return true;
}

void SGRectEx::log(const SGRect& rc) {
	CCLOG(":: 위치 [%d, %d], 크기 [%d, %d]\n", int(rc.getMinX()), int(rc.getMinY()), int(rc.getWidth()), int(rc.getHeight()));
}

bool SGRectEx::intersectY(const SGRect& lhs, const SGRect& rhs) {
	if (lhs.origin.y > rhs.origin.y &&
		lhs.origin.y < rhs.origin.y + rhs.size.height)
		return true;

	if (lhs.origin.y + lhs.size.height > rhs.origin.y &&
		lhs.origin.y + lhs.size.height < rhs.origin.y + rhs.size.height)
		return true;

	return false;
}

bool SGRectEx::containsX(const SGRect& rc, const float x) {
	return x > rc.origin.x && x < rc.origin.x + rc.size.width;
}

bool SGRectEx::containsY(const SGRect& rc, const float y) {
	return y > rc.origin.y && y < rc.origin.y + rc.size.height;
}



