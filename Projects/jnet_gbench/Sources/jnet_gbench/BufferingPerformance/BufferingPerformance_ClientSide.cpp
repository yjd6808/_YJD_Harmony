/*
 * 작성자: 윤정도
 * 생성일: 12/23/2023 9:19:50 PM
 * =====================
 *
 */


#include <jnet_gbench/Core.h>
#include <jnet_gbench/Cmdlist.h>

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
	void OnConnected(Session* _pSession) override
	{
		ConnectionCheck += 1;
	}

	void OnReceivedCmd(Session* _pSession, ICommand* _pCmd) override
	{
		if (_pCmd->GetId() != CMDID_SC_TEST)
		{
			jc_assert(false);
			return;
		}

		const int count = ++RecvCounter;

		if (count == TestSendCount * TestClientCount)
		{
			TestFinished.Signal();
		}
	}
};

struct tagClientGroup : NetGroup
{
	tagClientGroup()
		: NetGroup("클라이언트")
	{
	}

	void Initialize() override
	{
		CreateIOCP(TestClientCount * 2);
		CreateBufferPool({});
		RunIOCP();

		for (int i = 0; i < TestClientCount; ++i)
		{
			auto pClient = MakeShared<TcpClient>(pIocp_, pBufferPool_);
			pClient->SetEventListener(dbg_new ClientListener);
			pClient->SetHandle(i);
			AddHost(i, pClient);
			ClientList[i] = pClient.GetPtr();
		}
		finalized_ = false;
	}

	void ConnectToServer()
	{
		for (int i = 0; i < TestClientCount; ++i)
		{
			ClientList[i]->Connect(IPv4EndPoint::Parse(JNET_RESEARCH_CONN_ADDR));
		}
	}

	
} ClientGroup;


//////////////////////////////////////////////////////////////////////////////////////////
void ClientSide::Initialize()
{
	if (TestClientCount > MAX_CLIENT)
	{
		jc_assert_msg(false, "클라이언트 수가 너무 많습니다.");
		return;
	}

	ClientGroup.Initialize();
	ClientGroup.ConnectToServer();
}

//////////////////////////////////////////////////////////////////////////////////////////
void ClientSide::StartTest()
{
	if (ConnectionCheck != TestClientCount)
	{
		jc_assert(false);
		return;
	}

	Thread* pSendingThreads = dbg_new Thread[TestClientCount];

	for (int i = 0; i < TestClientCount; ++i)
	{
		pSendingThreads[i].Start([i](void*)
		{
			for (int j = 0; j < TestSendCount; ++j)
			{
				auto pPacket = dbg_new SingleCmdPacket<CS_TEST>();
				pPacket->cmd_.Seq = i * TestSendCount + j;
				ClientList[i]->SendAsync(pPacket);
			}
		});
	}

	for (int i = 0; i < TestClientCount; ++i)
	{
		pSendingThreads[i].Join();
	}

	delete[] pSendingThreads;
}

//////////////////////////////////////////////////////////////////////////////////////////
void ClientSide::Finalize()
{
	ClientGroup.Finalize();

	for (int i = 0; i < TestClientCount; ++i)
	{
		ClientList[i] = nullptr;
	}

	ConnectionCheck = 0;
	RecvCounter = 0;
}


NS_BUFFERING_PERFORMANCE_END