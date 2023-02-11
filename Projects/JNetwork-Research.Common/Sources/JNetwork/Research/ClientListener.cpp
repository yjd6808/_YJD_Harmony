/*
 * 작성자: 윤정도
 * 생성일: 2/11/2023 4:49:33 PM
 * =====================
 *
 */


#include "Research.h"
#include <JNetwork/Research/ClientListener.h>
#include <JNetwork/Research/Command.h>

#include <JNetwork/Packet/SendPacket.h>
#include <JCore/Utils/Console.h>

USING_NS_JC;

NS_JNET_BEGIN

ClientListener::ClientListener(const JCore::String& name) : m_Name(name) {}

void ClientListener::OnConnected() {
	SafeConsole::WriteLine("[%s] 연결되었습니다.", m_Name.Source());
}

void ClientListener::OnDisconnected() {
	SafeConsole::WriteLine("[%s] 연결이 종료되었습니다.", m_Name.Source());
}

void ClientListener::OnSent(ISendPacket* sentPacket, Int32UL sentBytes) {
	SafeConsole::WriteLine("[%s] 커맨드 %d개를 송신했습니다.", m_Name.Source(), sentPacket->GetCommandCount());
}

void ClientListener::OnReceived(ICommand* cmd) {
	if (cmd->GetCommand() == Cmd_SaticMessage)
		SafeConsole::WriteLine("[%s] 스태틱 메시지를 수신했습니다. : %s", m_Name.Source(), cmd->CastCommand<StaticMessage*>()->Msg.Source);
	else if (cmd->GetCommand() == Cmd_DynamicMessage) {
		DynamicMessage* pDynamicMessage = cmd->CastCommand<DynamicMessage*>();
		SafeConsole::WriteLine("[%s] 다이나믹 메시지를 수신했습니다. : %s(길이 : %d)", m_Name.Source(), pDynamicMessage->Msg, pDynamicMessage->Count - 1);
	}
}

void ClientListener::OnConnectFailed(Int32U errorMessage) {
	SafeConsole::WriteLine("[%s] 서버 접속에 실패했습니다 (오류코드: %u)", m_Name.Source(), errorMessage);
}

NS_JNET_END

