/*
 * 작성자: 윤정도
 * 생성일: 12/23/2023 9:19:50 PM
 * =====================
 *
 */


#include <Benchmark/Core.h>
#include <Benchmark/Cmdlist.h>

#include "PacketPooling_ServerSide.h"
#include "PacketPooling_Common.h"

USING_NS_JC;
USING_NS_JNET;

NS_PACKET_POOLING_BEGIN

static AtomicInt RecvCounter;

struct ServerListener : ServerEventListener
{
	void OnReceived(Session* session, ICommand* cmd) override {
		if (cmd->Cmd != CMDID_CS_TEST) {
			DebugAssert(false);
			return;
		}

		const int iAck = ++RecvCounter;

		if (Mode == TestMode::OnNetworking) {
			if (UsePooling && TlsMemPool == nullptr)
				TlsMemPool = MakeShared<BinarySearchMemoryPool>();

			auto pPacket = SinglePacket<SC_TEST>::Create(TlsMemPool);
			JNET_SEND_PACKET_AUTO_RELEASE_GUARD(pPacket);
			pPacket->Cmd.Ack = iAck;
			session->SendAsync(pPacket);
		} else if (Mode == TestMode::OnSending && iAck == TestSendCount * TestClientCount) {
			TestFinished.Signal();
		}
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
} ServerGroup;


void ServerSide::Initialize() {
	ServerGroup.Initialize();
}

void ServerSide::Finalize() {
	ServerGroup.Finalize();
	RecvCounter = 0;
}

NS_PACKET_POOLING_END