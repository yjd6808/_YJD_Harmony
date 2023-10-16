/*
 * 작성자: 윤정도
 * 생성일: 10/14/2023 12:08:26 PM
 * =====================
 *
 */

#include "Pch.h"
#include "L_GAME.h"

#include <Host/GameSession.h>

USING_NS_JNET;

void L_GAME::OnStarted() {
	_LogInfo_("서버가 시작되었습니다.");
}

void L_GAME::OnStartFailed(Int32U errorCode) {
	_LogInfo_("서버 시작 실패(%u)", errorCode);
}

void L_GAME::OnStopped() { _LogInfo_("서버가 중지되었습니다.");  }
void L_GAME::OnConnected(Session* session) { _LogInfo_("%s가 접속하였습니다.", session->GetRemoteEndPoint().ToString().Source()); }
void L_GAME::OnConnectFailed(Session* session, Int32U errorCode) {}
void L_GAME::OnDisconnected(Session* session) {
	_LogInfo_("%s가 연결 종료하였습니다.", session->GetRemoteEndPoint().ToString().Source());
	GameSession* pSession = (GameSession*)session;
	Player* pPlayer = pSession->GetPlayer();
	if (pPlayer == nullptr) {
		return;
	}

	pPlayer->OnDisconnected();
	pSession->SetPlayer(nullptr);
}

void L_GAME::OnSent(Session* session, ISendPacket* sentPacket, Int32UL sentBytes) {
	sentPacket->ForEach([](ICommand* cmd) { _LogInfo_("%s(%d) 송신", Core::CommandNameMap.Get(cmd->GetCommand()), cmd->GetCommand()); });
}

void L_GAME::OnReceived(Session* session, ICommand* recvCmd) {
	Core::NetCore->RunCommand(session, recvCmd);
}
void L_GAME::OnReceived(Session* session, IRecvPacket* recvPacket) { }