/*
 * 작성자: 윤정도
 * 생성일: 10/13/2023 3:11:56 PM
 * =====================
 *
 */


#pragma once


#include <TF/Common/Const.h>

NS_CONST_BEGIN

namespace Resource
{
	extern const char* FontName;
	extern const char* WhiteRectFileName;			// 흰색 사각형 판때기
	extern const char* TransparentRectFileName;		// 투명 사각형 판때기
}

namespace Ping
{
	extern const float Delay;
}

namespace Layer
{
	extern const int ID_Grid;
	extern const int ID_UI;
}

namespace Window
{
	extern const float Width;
	extern const float Height;
}

NS_CONST_END