/*
 * 작성자: 윤정도
 * 생성일: 3/23/2023 12:42:54 AM
 * =====================
 * 결과는 코코스 좌표계를 기준으로한다.
 */


#pragma once

#include <sgcl/Core.h>

struct Win32Helper
{
	inline static cocos2d::GLViewImpl* View;
	inline static cocos2d::Director* Director;
	inline static HWND ViewHandle;

	static void LazyInit();

	static c2d::vec2 GetWindowPos();
	static c2d::rect GetWindowRect();
	static c2d::vec2 GetCursorPos();
};
