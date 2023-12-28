/*
 * 작성자: 윤정도
 * 생성일: 12/23/2023 9:19:50 PM
 * =====================
 *
 */


#include <JNetwork/Network.h>
#include <JNetwork/Host/TcpClient.h>
#include <JNetwork/NetGroup.h>

#include "ClientSide.h"
#include "Common.h"

USING_NS_JC;
USING_NS_JNET;

int ConnectionCheck_v = 0;
AtomicInt RecvCounter_v = 0;
StopWatch<StopWatchMode::HighResolution> StopWatch_v;
TcpClient* ClientList_v[TEST_CLIENT_COUNT];

struct ClientListener : ClientEventListener
{
	void OnConnected(Session* session) override {
		ConnectionCheck_v += 1;

		if (ConnectionCheck_v == TEST_CLIENT_COUNT) {
			Thread sendingThreads[TEST_CLIENT_COUNT];
			StopWatch_v.Start();

			for (int i = 0; i < TEST_CLIENT_COUNT; ++i) {
				sendingThreads[i].Start([i](void*) {
					for (int j = 0; j < TEST_SEND_COUNT; ++j) {
						auto pPacket = dbg_new SinglePacket<CS_TEST>();
						JNET_SEND_PACKET_AUTO_RELEASE_GUARD(pPacket);
						pPacket->Cmd.Seq = i * TEST_SEND_COUNT + j;
						ClientList_v[i]->SendAsync(pPacket);
					}
				});
			}

			for (int i = 0; i < TEST_CLIENT_COUNT; ++i) {
				sendingThreads[i].Join();
			}

			Console::WriteLine("%d회 에코 송신 완료", TEST_SEND_COUNT * TEST_CLIENT_COUNT);
		}
	}

	void OnReceived(Session* session, ICommand* cmd) override {
		if (cmd->Cmd != CMDID_SC_TEST) {
			DebugAssert(false);
			return;
		}

		const int iCount = ++RecvCounter_v;

		if (iCount == TEST_SEND_COUNT * TEST_CLIENT_COUNT) {
			Console::WriteLine("%d회 에코 수신 완료 (%.2lf초)", TEST_SEND_COUNT * TEST_CLIENT_COUNT, StopWatch_v.Stop().GetTotalSeconds());
			TestFinished_v = true;
		}
	}
};

struct tagClientGroup : NetGroup
{
	tagClientGroup() : NetGroup("클라이언트") {}

	void Initialize() override {
		CreateIocp(TEST_CLIENT_COUNT * 2);
		CreateBufferPool({});
		RunIocp();

		for (int i = 0; i < TEST_CLIENT_COUNT; ++i) {
			auto spClient = MakeShared<TcpClient>(m_spIOCP, m_spBufferPool);
			spClient->SetEventListener(dbg_new ClientListener);
			spClient->SetHandle(i);
			AddHost(i, spClient);
			ClientList_v[i] = spClient.GetPtr();
		}
		m_bFinalized = false;
	}

	void ConnectToServer() {
		for (int i = 0; i < TEST_CLIENT_COUNT; ++i) {
			ClientList_v[i]->Connect(IPv4EndPoint::Parse(JNET_RESEARCH_CONN_ADDR));
		}
	}

	
} ClientGroup_v;


void ClientSide::Initialize() {
	ClientGroup_v.Initialize();
	ClientGroup_v.ConnectToServer();
}

void ClientSide::Finalize() {
	ClientGroup_v.Finalize();

	for (int i = 0; i < TEST_CLIENT_COUNT; ++i) {
		ClientList_v[i] = nullptr;
	}

	ConnectionCheck_v = 0;
	RecvCounter_v = 0;
}
