/*
 * 작성자: 윤정도
 * 생성일: 12/23/2023 9:19:50 PM
 * =====================
 *
 */


#include <jnet_gbench/Core.h>
#include <jnet_gbench/Cmdlist.h>

#include "BufferingWithNagle_ServerSide.h"
#include "BufferingWithNagle_Common.h"

USING_NS_JC;
USING_NS_JNET;

NS_BUFFERING_WITH_NAGLE_BEGIN

static AtomicInt RecvCounter;

struct ServerListener : ServerEventListener
{
	//////////////////////////////////////////////////////////////////////////////////////
	void OnReceived(Session* _pSession, ICommand* _pCmd) override
	{
		if (_pCmd->GetId() != CMDID_CS_TEST)
		{
			jc_assert(false);
			return;
		}

		const int ack = ++RecvCounter;
		if (ack == TestSendCount * TestClientCount)
		{
			TestFinished.Signal();
		}
	}
};

struct tagServerGroup : NetGroup
{
	tagServerGroup()
		: NetGroup("서버")
	{
	}

	//////////////////////////////////////////////////////////////////////////////////////
	void Initialize() override
	{
		CreateIOCP(8);
		CreateBufferPool({});
		RunIOCP();
		auto pServer = MakeShared<TcpServer>(pIocp_, pBufferPool_);
		pServer->SetEventListener(dbg_new ServerListener);
		pServer->Start(IPv4EndPoint::Parse(JNET_RESEARCH_BIND_ADDR));
		AddHost(0, pServer);
		finalized_ = false;

	}
} ServerGroup_v;


////////////////////////////////////////////////////////////////////////////////////////
void ServerSide::Initialize()
{
	ServerGroup_v.Initialize();
}

////////////////////////////////////////////////////////////////////////////////////////
void ServerSide::Finalize()
{
	ServerGroup_v.Finalize();
	RecvCounter = 0;
}

NS_BUFFERING_WITH_NAGLE_END