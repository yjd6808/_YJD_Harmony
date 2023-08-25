/*
 * 작성자: 윤정도
 * 생성일: 8/25/2023 6:59:41 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/Namespace.h>

NS_CALLBACK_BEGIN

namespace Popup
{
	struct LobbyAuthenticationTimeout
	{
		void operator()();
	};
}

NS_CALLBACK_END