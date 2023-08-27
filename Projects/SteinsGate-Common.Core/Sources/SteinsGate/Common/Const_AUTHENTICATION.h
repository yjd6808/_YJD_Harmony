/*
 * 작성자: 윤정도
 * 생성일: 8/27/2023 7:21:29 PM
 * =====================
 *
 */


#pragma once

#include <JCore/Primitives/Range.h>
#include <SteinsGate/Common/Namepsace.h>

NS_CONST_BEGIN

namespace Authentication
{
	constexpr JCore::Range SerialRange = { 10000000, 20000000 };
}

namespace Timeout
{
	constexpr int LobbyWait		= 15;
	constexpr int Lobby			= 900;
	constexpr int GameWait		= 15;
	constexpr int Game			= 1800;
}


NS_CONST_END

