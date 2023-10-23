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
	extern const char* WhiteCircleFileName;
	extern const char* BigWhiteCircleFileName;
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

namespace String
{
	extern const char* PlayWaitLeftTimeFormat;
	extern const char* PlayingLeftTimeFormat;
	extern const char* EndWaitLeftTimeFormat;
	extern const char* RevivalLeftTimeFormat;
}

namespace Window
{
	extern const float Width;
	extern const float Height;
	extern const char ViewName[];
}

namespace Properties
{
	namespace GameClient
	{
		extern const int Reconnecting;
		extern const int AccountPrimaryKey;
		extern const int AccountIdKey;
		extern const int ChannelPrimaryKey;
		extern const int PlayerState;
		extern const int RoomAccessId;
		extern const int IsIntruder;
	}
}



NS_CONST_END