/*
 * 작성자: 윤정도
 * 생성일: 12/23/2023 9:19:50 PM
 * =====================
 *
 */


#include <Benchmark/Core.h>
#include <Benchmark/Cmdlist.h>

#include "BufferingWithNagle_ClientSide.h"
#include "BufferingWithNagle_Common.h"


USING_NS_JC;
USING_NS_JNET;

#define MAX_CLIENT	64

NS_BUFFERING_WITH_NAGLE_BEGIN

int ConnectionCheck = 0;
AtomicInt RecvCounter = 0;
TcpClient* ClientList[MAX_CLIENT];

struct ClientListener : ClientEventListener
{
	void OnConnected(Session* session) override {
		ConnectionCheck += 1;
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

	void SetNagle(bool nagle) {
		for (int i = 0; i < TestClientCount; ++i) {
			if (ClientList[i]->Socket().Option().SetNagleEnabled(nagle) == SOCKET_ERROR) {
				DebugAssert(false);
			}
		}
	}

	void ConnectToServer() {
		for (int i = 0; i < TestClientCount; ++i) {
			ClientList[i]->Connect(IPv4EndPoint::Parse(JNET_RESEARCH_CONN_ADDR));
		}
	}

	
} ClientGroup;


void ClientSide::Initialize(bool nagle) {
	if (TestClientCount > MAX_CLIENT) {
		DebugAssertMsg(false, "클라이언트 수가 너무 많습니다.");
		return;
	}

	ClientGroup.Initialize();
	ClientGroup.SetNagle(nagle);
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

				if (AsyncSending)
					ClientList[i]->SendAsync(pPacket);
				else
					ClientList[i]->Send(pPacket, false);
			}

			if (AsyncSending)
				return;

			// 동기 송신인 경우 버퍼에 남아있는거 다 보냄
			for (;;) {
				Int32U e;
				ClientList[i]->SendPending(e);

				if (!ClientList[i]->HasPendingData())
					break;
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


NS_BUFFERING_WITH_NAGLE_END