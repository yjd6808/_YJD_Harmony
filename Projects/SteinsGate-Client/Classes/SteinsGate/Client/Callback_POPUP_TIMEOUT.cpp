/*
 * 작성자: 윤정도
 * 생성일: 8/25/2023 7:02:57 PM
 * =====================
 *
 */


#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "Callback_POPUP_TIMEOUT.h"

#include <SteinsGate/Client/AuthenticationComponent.h>


NS_CALLBACK_BEGIN

namespace Popup
{
	void LobbyAuthenticationTimeout::operator()() {
		if (Core::Net->getAuthenticationComponent()->getState() == AuthenticationState::LobbyWait) {
			
		}
	}


}

NS_CALLBACK_END