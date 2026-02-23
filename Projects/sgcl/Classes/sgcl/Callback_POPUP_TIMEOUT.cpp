/*
 * 작성자: 윤정도
 * 생성일: 8/25/2023 7:02:57 PM
 * =====================
 *
 */


#include "Core.h"
#include "Callback_POPUP_TIMEOUT.h"

#include <sgcl/_Net/NetCore.h>
#include <sgcl/AuthenticationComponent.h>


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
