/*
 * 작성자: 윤정도
 * 생성일: 1/11/2023 5:02:45 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/Namespace.h>

NS_CONST_BEGIN

namespace Timeout
{
	constexpr float Login = 5.0f;
	constexpr float LobbyConnection = 5.0f;
	constexpr float LobbyAuthentication = 15.0f;
}

NS_CONST_END