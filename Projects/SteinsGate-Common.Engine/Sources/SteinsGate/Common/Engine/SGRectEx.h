/*
 * 작성자: 윤정도
 * 생성일: 1/16/2023 3:00:20 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/Engine.h>
#include <JCore/Math.h>

class SGRectEx : SGRect
{
public:
	/**
	 * \brief 인터섹트 결과와 더불어 겹쳐진 영역의 위치를 반환하도록 한다.
	 * 하단 테스트코드 참고
	 */
	static bool intersect(const SGRect& rc, const SGRect& rc2, SGRect& intersectRect) {
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

    static void log(const SGRect& rc) {
        Log(":: 위치 [%d, %d], 크기 [%d, %d]\n", int(rc.getMinX()), int(rc.getMinY()), int(rc.getWidth()), int(rc.getHeight()));
	}

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


     static bool intersectY(const SGRect& lhsThick, const SGRect& rhsThick) {
        if (lhsThick.origin.y > rhsThick.origin.y &&
            lhsThick.origin.y < rhsThick.origin.y + rhsThick.size.height)
            return true;

        if (lhsThick.origin.y + lhsThick.size.height > rhsThick.origin.y &&
            lhsThick.origin.y + lhsThick.size.height < rhsThick.origin.y + rhsThick.size.height)
            return true;

        return false;
     }
};