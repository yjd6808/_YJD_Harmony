/*
 * 작성자: 윤정도
 * 생성일: 4/19/2023 3:23:52 PM
 * =====================
 *
 */

#include "Auth.h"
#include "AuthCoreHeader.h"
#include "S_AUTHENTICATION_IS.h"

#include <SteinsGate/Common/CmdRelay_AUTHENTICATION.h>

//////////////////////////////////////////////////////////////////////////////////////////
void S_AUTHENTICATION_IS::SEND_AUS_AuthenticationCheckAck(bool _success, int _sessionHandle, GameServerType_t _lastServer)
{
	const auto sending = SendBegin<AUS_AuthenticationCheckAck>();
	sending.cmd_.Success = _success;
	sending.cmd_.SessionHandle = _sessionHandle;
	sending.cmd_.LastServer = _lastServer;
}
