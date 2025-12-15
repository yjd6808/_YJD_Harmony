/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 8:34:29 AM [SteinsGate-Server.Auth 프로젝트 복사 생성]
 * =====================
 *
 */


#include "Lobby.h"
#include "LobbyCoreHeader.h"
#include "ListenerInterServerHostUdp.h"

#include <SteinsGate/Common/InterServerCmd_HOST.h>

//////////////////////////////////////////////////////////////////////////////////////////
ListenerInterServerHostUdp::ListenerInterServerHostUdp()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void ListenerInterServerHostUdp::OnReceived(SGSession* _pSession, JNetwork::ICommand* _pCommand)
{
	ListenerClientCommon::OnReceived(_pSession, _pCommand);
}
