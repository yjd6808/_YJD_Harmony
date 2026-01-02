/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 9:10:29 AM [SteinsGate-Server.Lobby 프로젝트 복사 생성]
 * =====================
 *
 */


#include "Game.h"
#include "GameCoreHeader.h"
#include "ListenerInterServerHostUdp.h"

#include <SteinsGate/Common/InterServerCmd_HOST.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

ListenerInterServerHostUdp::ListenerInterServerHostUdp()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void ListenerInterServerHostUdp::OnReceived(SGSession* _pSession, JNetwork::ICommand* _pCommand)
{
	ListenerClientCommon::OnReceived(_pSession, _pCommand);
}
