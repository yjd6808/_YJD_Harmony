/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 8:34:29 AM [SteinsGate-Server.Auth 프로젝트 복사 생성]
 * =====================
 *
 */

#include "Core.h"
#include "LobbyCoreHeader.h"
#include "S_AUTHENTICATION_IS.h"

#include <sgs/CmdRelay_AUTHENTICATION.h>

USING_NS_JC;
USING_NS_JNET;

//////////////////////////////////////////////////////////////////////////////////////////
void S_AUTHENTICATION_IS::SEND_SAU_AuthenticationCheck(int _sessionHandle, const char* _pAccountId, AuthenticationSerial_t _serial)
{
	auto sending = SendBegin<SAU_AuthenticationCheck>();
	sending.cmd_.SessionHandle = _sessionHandle;
	sending.cmd_.AccountId.SetString(_pAccountId);
	sending.cmd_.RequestedServer = ServerProcessType::Lobby;
	sending.cmd_.Serial = _serial;
}
