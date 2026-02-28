/*
 * 작성자: 윤정도
 * 생성일: 12/23/2023 9:19:50 PM
 * =====================
 *
 */


#include <jnet_gbench/Core.h>
#include <jnet_gbench/Cmdlist.h>

#include "PacketPooling_ServerSide.h"
#include "PacketPooling_Common.h"

USING_NS_JC;
USING_NS_JNET;

NS_PACKET_POOLING_BEGIN

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

		if (Mode == TestMode::OnNetworking)
		{
			if (UsePooling && TlsMemPool == nullptr)
				TlsMemPool = MakeShared<CBinarySearchMemoryPool>();

			auto pPacket = SingleCmdPacket<SC_TEST>::Create(TlsMemPool);
			JNET_SEND_PACKET_AUTO_RELEASE_GUARD(pPacket);
			pPacket->cmd_.Ack = ack;
			_pSession->SendAsync(pPacket);
		}
		else if (Mode == TestMode::OnSending && ack == TestSendCount * TestClientCount)
		{
			TestFinished.Signal();
		}
	}
};

struct tagServerGroup : NetGroup
{
	//////////////////////////////////////////////////////////////////////////////////////
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
} ServerGroup;

//////////////////////////////////////////////////////////////////////////////////////////
void ServerSide::Initialize()
{
	ServerGroup.Initialize();
}

//////////////////////////////////////////////////////////////////////////////////////////
void ServerSide::Finalize()
{
	ServerGroup.Finalize();
	RecvCounter = 0;
}

NS_PACKET_POOLING_END
