/*
 * 작성자: 윤정도
 * 생성일: 3/23/2023 12:42:54 AM
 * =====================
 * 결과는 코코스 좌표계를 기준으로한다.
 */


#pragma once

#include <SteinsGate/Client/Tutturu.h>

struct Win32Helper
{
	inline static cocos2d::GLViewImpl* View;
	inline static cocos2d::Director* Director;
	inline static HWND ViewHandle;

	static void LazyInit();

	static SGVec2 getWindowPos();
	static SGRect getWindowRect();
	static SGVec2 getCursorPos();
};

