/*
 * 작성자: 윤정도
 * 생성일: 2/11/2023 4:49:33 PM
 * =====================
 *
 */


#include "Research.h"
#include <JNetwork/Research/ServerListener.h>
#include <JNetwork/Research/Command.h>

#include <JNetwork/Packet/SendPacket.h>
#include <JNetwork/Host/Session.h>

#include <JCore/Primitives/StringUtil.h>
#include <JCore/Utils/Console.h>

USING_NS_JC;

NS_JNET_BEGIN

ServerListener::ServerListener(const JCore::String& name) : m_Name(name) {}

void ServerListener::OnStarted() {
	Console::WriteLine("[%s] 서버가 시작되었습니다.", m_Name.Source());
}

void ServerListener::OnConnected(Session* connectedSession) {
	Console::WriteLine("[%s] %s 클라이언트가 접속하였습니다.",
		m_Name.Source(), connectedSession->GetRemoteEndPoint().ToString().Source());
}

void ServerListener::OnDisconnected(Session* disconnetedSession) {
	Console::WriteLine("[%s] %s 클라이언트가 접속해제하였습니다.",
		m_Name.Source(), disconnetedSession->GetRemoteEndPoint().ToString().Source());
}

void ServerListener::OnSent(Session* sender, ISendPacket* packet, Int32UL sentBytes) {
	//Console::WriteLine("[%s] 송신 : %d 바이트", m_Name.Source(), packet->GetPacketLength());
}

void ServerListener::OnReceived(Session* receiver, ICommand* cmd) {
	// 수신한 메시지 출력
	if (cmd->GetCommand() == Cmd_SaticMessage) {
		StaticMessage* pMsg = cmd->CastCommand<StaticMessage*>();
		Console::WriteLine("[%s] 스태틱 메시지를 수신했습니다. : %s", m_Name.Source(), pMsg->Msg.Source);

		// 스태틱 패킷 에코 진행
		auto pPacket = dbg_new StaticPacket<StaticMessage>();
		StaticMessage* arg1 = pPacket->Get<0>();
		arg1->Msg.SetString(pMsg->Msg);

		if (!receiver->SendAsync(pPacket)) {
			Console::WriteLine("[%s] 스태틱 에코 실패", m_Name.Source());
			return;
		}

		Console::WriteLine("[%s] 스태틱 에코", m_Name.Source());
	} else if (cmd->GetCommand() == Cmd_DynamicMessage) {
		DynamicMessage* pDynamicMessage = cmd->CastCommand<DynamicMessage*>();
		Dummy* pDummy = &pDynamicMessage->dmg;

		if (pDummy->a != 1 || pDummy->b != 2 || pDummy->c != 3 || pDynamicMessage->d != 4) {
			Console::WriteLine("데이터를 올바르게 수신하지 못했습니다.");
			return;
		}

		int iLen = pDynamicMessage->Count - 1;
		Console::WriteLine("[%s] 다이나믹 메시지를 수신했습니다. : %s(길이 : %d)", m_Name.Source(), pDynamicMessage->Msg, iLen);

		// 다이나믹 패킷 에코 진행
		auto pPacket = dbg_new DynamicPacket<DynamicMessage>(iLen + 1);
		DynamicMessage* pMsg = pPacket->Get<0>();
		StringUtil::CopyUnsafe(pMsg->Msg, pDynamicMessage->Msg);

		if (!receiver->SendAsync(pPacket)) {
			Console::WriteLine("[%s] 다이나믹 에코 실패", m_Name.Source());
			return;
		}

		Console::WriteLine("[%s] 다이나믹 에코", m_Name.Source());
	}
}


void ServerListener::OnStopped() {
	Console::WriteLine("[%s] 종료되었습니다.", m_Name.Source());
}

NS_JNET_END


