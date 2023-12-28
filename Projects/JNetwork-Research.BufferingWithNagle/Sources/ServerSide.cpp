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

		const int iAck = ++RecvCounter_v;

		if (!DontEcho_v) {
			auto pPacket = dbg_new SinglePacket<SC_TEST>();
			JNET_SEND_PACKET_AUTO_RELEASE_GUARD(pPacket);
			pPacket->Cmd.Ack = iAck;
#if ASYNC_SENDING
			session->SendAsync(pPacket);
#else
			session->Send(pPacket, false);
#endif
		} else if (iAck == TEST_SEND_COUNT * TEST_CLIENT_COUNT) {
			LastTime_v = StopWatch_v.Stop();
			Console::WriteLine("%d회 수신 완료 (%.2lf초)", TEST_SEND_COUNT * TEST_CLIENT_COUNT, LastTime_v.GetTotalSeconds());
			TestFinished_v = true;
		}
	}

	void OnDisconnected(Session* session, Int32U errorCode) override {
		//Console::Write("[서버] %s와 연결이 끊어짐(%s", session->GetRemoteEndPoint().ToString().Source(), Winsock::ErrorMessageUTF8(errorCode));
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
