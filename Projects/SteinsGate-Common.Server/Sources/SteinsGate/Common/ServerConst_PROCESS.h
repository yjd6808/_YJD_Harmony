/*
 * 작성자: 윤정도
 * 생성일: 8/26/2023 2:01:14 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/ServerNamespace.h>

NS_CONST_BEGIN

namespace Process
{
	constexpr int LoopPerSecondAuth = 10;
	constexpr int LoopPerSecondLobby = 5;
	constexpr int LoopPerSecondGame = 4;
	constexpr int LoopPerSecondCenter = 10;
}

NS_CONST_END
