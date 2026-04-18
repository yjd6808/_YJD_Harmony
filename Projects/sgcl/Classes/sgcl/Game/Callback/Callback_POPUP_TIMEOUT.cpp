/*
 * 작성자: 윤정도
 * 생성일: 8/25/2023 7:02:57 PM
 * =====================
 *
 */


#include "Game/Callback/Callback_POPUP_TIMEOUT.h"

#include "sgcl/Net/NetCore.h"
#include "sgcl/Net/Component/AuthenticationComponent.h"


NS_CALLBACK_BEGIN
namespace Popup
{
//////////////////////////////////////////////////////////////////////////////////////
void LobbyAuthenticationTimeout::operator()()
{
	if (g_cNet.authentication_.GetState() == AuthenticationState::LobbyWait)
	{
	}
}
}

NS_CALLBACK_END
