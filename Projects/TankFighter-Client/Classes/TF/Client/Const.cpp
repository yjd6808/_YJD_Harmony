/*
 * 작성자: 윤정도
 * 생성일: 10/13/2023 3:20:06 PM
 * =====================
 *
 */


#include "Pch.h"
#include "Const.h"

NS_CONST_BEGIN

namespace Resource
{
	const char* FontName = "fonts/N2G.ttf";
	const char* WhiteRectFileName = "white_rect.png";
	const char* TransparentRectFileName = "transparent_rect.png";
	const char* WhiteCircleFileName = "white_circle.png";
	const char* BigWhiteCircleFileName = "white_circle_big.png";
}

namespace Ping
{
	const float Delay = 0.5f;
}

namespace Layer
{
	const int ID_UI = 100;
	const int ID_Grid = 101;
}

namespace String
{
	const char* PlayWaitLeftTimeFormat = "게임 시작까지 남은시간 : %.1f초";
	const char* PlayingLeftTimeFormat = "게임 종료까지 남은시간 : %.1f초";
	const char* EndWaitLeftTimeFormat = "방으로 이동하기까지 남은시간 : % .1f초";
	const char* RevivalLeftTimeFormat = "%.1f초뒤 부활합니다.";
}

namespace Window
{
	const float Width = 1000;
	const float Height = 600;
	const char ViewName[] = "탱크파이터";
}

namespace Properties
{
	namespace GameClient
	{
		const int Reconnecting = 1;
		const int AccountPrimaryKey = 2;
		const int AccountIdKey = 3;
		const int ChannelPrimaryKey = 4;
		const int PlayerState = 5;
		const int RoomAccessId = 6;
		const int IsIntruder = 7;
	}
}

NS_CONST_END