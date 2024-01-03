/*
 * 작성자: 윤정도
 * 생성일: 12/23/2023 9:19:50 PM
 * =====================
 *
 */


#include <Benchmark/Core.h>
#include <Benchmark/Cmdlist.h>

#include "BufferingWithNagle_ServerSide.h"
#include "BufferingWithNagle_Common.h"

USING_NS_JC;
USING_NS_JNET;

NS_BUFFERING_WITH_NAGLE_BEGIN

static AtomicInt RecvCounter;

struct ServerListener : ServerEventListener
{
	void OnReceived(Session* session, ICommand* cmd) override {
		if (cmd->Cmd != CMDID_CS_TEST) {
			DebugAssert(false);
			return;
		}

		const int iAck = ++RecvCounter;
		if (iAck == TestSendCount * TestClientCount) {
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
} ServerGroup_v;


void ServerSide::Initialize() {
	ServerGroup_v.Initialize();
}

void ServerSide::Finalize() {
	ServerGroup_v.Finalize();
	RecvCounter = 0;
}

NS_BUFFERING_WITH_NAGLE_END