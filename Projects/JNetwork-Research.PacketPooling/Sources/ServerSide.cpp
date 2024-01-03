/*
 * 작성자: 윤정도
 * 생성일: 12/23/2023 9:19:50 PM
 * =====================
 *
 */


#include <JNetwork/Network.h>
#include <JNetwork/Winsock.h>
#include <JNetwork/Host/TcpServer.h>
#include <JNetwork/NetGroup.h>

#include "ServerSide.h"
#include "Common.h"

USING_NS_JC;
USING_NS_JNET;

static AtomicInt RecvCounter_v;

struct ServerListener : ServerEventListener
{
	void OnReceived(Session* session, ICommand* cmd) override {
		if (cmd->Cmd != CMDID_CS_TEST) {
			DebugAssert(false);
			return;
		}

		if (UsePooling_v && Pool_v == nullptr)
			Pool_v = MakeShared<BinarySearchMemoryPool>();

		const int iAck = ++RecvCounter_v;
		auto pPacket = SinglePacket<SC_TEST>::Create(UsePooling_v ? Pool_v : nullptr);
		JNET_SEND_PACKET_AUTO_RELEASE_GUARD(pPacket);
		pPacket->Cmd.Ack = iAck;
		session->SendAsync(pPacket);
	}

	void OnDisconnected(Session* session, Int32U errorCode) override {
		// Console::Write("[서버] %s와 연결이 끊어짐(%s", session->GetRemoteEndPoint().ToString().Source(), Winsock::ErrorMessageUTF8(errorCode));
	}
};

struct tagServerGroup : NetGroup
{
	tagServerGroup() : NetGroup("서버") {}

	void Initialize() override {
		CreateIocp(8);
		CreateBufferPool({});
		RunIocp();
		auto spServer = MakeShared<TcpServer>(m_spIOCP, m_spBufferPool);
		spServer->SetEventListener(dbg_new ServerListener);
		spServer->Start(IPv4EndPoint::Parse(JNET_RESEARCH_BIND_ADDR));
		AddHost(0, spServer);
		m_bFinalized = false;

	}
} ServerGroup_v;


void ServerSide::Initialize() {
	ServerGroup_v.Initialize();
}

void ServerSide::Finalize() {
	ServerGroup_v.Finalize();
	RecvCounter_v = 0;
}
