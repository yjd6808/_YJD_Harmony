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
	}
}

NS_CONST_END