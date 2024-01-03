/*
 * 작성자: 윤정도
 * 생성일: 12/23/2023 9:19:50 PM
 * =====================
 *
 */


#include <Benchmark/Core.h>
#include <Benchmark/Cmdlist.h>

#include "BufferingPerformance_ClientSide.h"
#include "BufferingPerformance_Common.h"


USING_NS_JC;
USING_NS_JNET;

#define MAX_CLIENT	64

NS_BUFFERING_PERFORMANCE_BEGIN

int ConnectionCheck = 0;
AtomicInt RecvCounter = 0;
TcpClient* ClientList[MAX_CLIENT];

struct ClientListener : ClientEventListener
{
	void OnConnected(Session* session) override {
		ConnectionCheck += 1;
	}

	void OnReceived(Session* session, ICommand* cmd) override {
		if (cmd->Cmd != CMDID_SC_TEST) {
			DebugAssert(false);
			return;
		}

		const int iCount = ++RecvCounter;

		if (iCount == TestSendCount * TestClientCount) {
			TestFinished.Signal();
		}
	}
};

struct tagClientGroup : NetGroup
{
	tagClientGroup() : NetGroup("클라이언트") {}

	void Initialize() override {
		CreateIocp(TestClientCount * 2);
		CreateBufferPool({});
		RunIocp();

		for (int i = 0; i < TestClientCount; ++i) {
			auto spClient = MakeShared<TcpClient>(m_spIOCP, m_spBufferPool);
			spClient->SetEventListener(dbg_new ClientListener);
			spClient->SetHandle(i);
			AddHost(i, spClient);
			ClientList[i] = spClient.GetPtr();
		}
		m_bFinalized = false;
	}

	void ConnectToServer() {
		for (int i = 0; i < TestClientCount; ++i) {
			ClientList[i]->Connect(IPv4EndPoint::Parse(JNET_RESEARCH_CONN_ADDR));
		}
	}

	
} ClientGroup;


void ClientSide::Initialize() {
	if (TestClientCount > MAX_CLIENT) {
		DebugAssertMsg(false, "클라이언트 수가 너무 많습니다.");
		return;
	}

	ClientGroup.Initialize();
	ClientGroup.ConnectToServer();
}

void ClientSide::StartTest() {
	if (ConnectionCheck != TestClientCount) {
		DebugAssert(false);
		return;
	}


	Thread* pSendingThreads = dbg_new Thread[TestClientCount];

	for (int i = 0; i < TestClientCount; ++i) {
		pSendingThreads[i].Start([i](void*) {
			for (int j = 0; j < TestSendCount; ++j) {
				auto pPacket = dbg_new SinglePacket<CS_TEST>();
				JNET_SEND_PACKET_AUTO_RELEASE_GUARD(pPacket);
				pPacket->Cmd.Seq = i * TestSendCount + j;
				ClientList[i]->SendAsync(pPacket);
			}
		});
	}

	for (int i = 0; i < TestClientCount; ++i) {
		pSendingThreads[i].Join();
	}

	delete[] pSendingThreads;
}

void ClientSide::Finalize() {
	ClientGroup.Finalize();

	for (int i = 0; i < TestClientCount; ++i) {
		ClientList[i] = nullptr;
	}

	ConnectionCheck = 0;
	RecvCounter = 0;
}


NS_BUFFERING_PERFORMANCE_END