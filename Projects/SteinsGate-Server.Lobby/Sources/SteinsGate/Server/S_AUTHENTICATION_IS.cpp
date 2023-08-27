/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 8:34:29 AM [SteinsGate-Server.Auth 프로젝트 복사 생성]
 * =====================
 *
 */

#include "Lobby.h"
#include "LobbyCoreHeader.h"
#include "S_AUTHENTICATION_IS.h"

#include <SteinsGate/Common/CmdRelay_AUTHENTICATION.h>

USING_NS_JC;
USING_NS_JNET;

void S_AUTHENTICATION_IS::SEND_SAU_AuthenticationCheck(int sessionHandle, AuthenticationSerial_t serial) {
	auto sending = SendBegin<SAU_AuthenticationCheck>();
	sending.Cmd.SessionHandle = sessionHandle;
	sending.Cmd.RequestedServer = ServerProcessType::Lobby;
	sending.Cmd.Serial = serial;
}
