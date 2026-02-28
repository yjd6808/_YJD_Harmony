/*
 * 작성자: 윤정도
 * 생성일: 2/11/2023 4:49:33 PM
 * =====================
 *
 */


#include <jnetr_common/Core.h>
#include <jnetr_common/ServerListener.h>
#include <jnetr_common/Command.h>

#include <jnet/Host/Session.h>

#include <jc/Primitives/StringUtil.h>
#include <jc/Utils/Console.h>

USING_NS_JC;

NS_JNET_BEGIN
//////////////////////////////////////////////////////////////////////////////////////////
ServerListener::ServerListener(const jc::String& _name)
: name_(_name)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void ServerListener::OnStarted()
{
	Console::WriteLine("[%s] 서버가 시작되었습니다.", name_.Source());
}

//////////////////////////////////////////////////////////////////////////////////////////
void ServerListener::OnConnected(Session* _pConnectedSession)
{
	Console::WriteLine("[%s] %s 클라이언트가 접속하였습니다.",
	                   name_.Source(), _pConnectedSession->GetRemoteEndPoint().ToString().Source());
}

//////////////////////////////////////////////////////////////////////////////////////////
void ServerListener::OnDisconnected(Session* _pDisconnectedSession, _u32 _errorCode)
{
	Console::WriteLine("[%s] %s 클라이언트가 접속해제하였습니다.",
	                   name_.Source(), _pDisconnectedSession->GetRemoteEndPoint().ToString().Source());
}

//////////////////////////////////////////////////////////////////////////////////////////
void ServerListener::OnSent(Session* _pSender, IPacket* _pPacket, _u32l _sentBytes)
{
	//Console::WriteLine("[%s] 송신 : %d 바이트", name_.Source(), _pPacket->GetPacketLength());
}

//////////////////////////////////////////////////////////////////////////////////////////
void ServerListener::OnReceived(Session* _pReceiver, ICommand* _pCmd)
{
	// 수신한 메시지 출력
	if (_pCmd->GetId() == CMD_SATIC_MESSAGE)
	{
		StaticMessage* pMsg = _pCmd->CastCommand<StaticMessage*>();
		Console::WriteLine("[%s] 스태틱 메시지를 수신했습니다. : %s", name_.Source(), pMsg->msg_.Source);

		// 스태틱 패킷 에코 진행
		auto pPacket = dbg_new StaticCmdPacket<StaticMessage>();
		StaticMessage* pArg1 = pPacket->Get<0>();
		pArg1->msg_.SetString(pMsg->msg_);

		if (!_pReceiver->SendAsync(pPacket))
		{
			Console::WriteLine("[%s] 스태틱 에코 실패", name_.Source());
			return;
		}

		Console::WriteLine("[%s] 스태틱 에코", name_.Source());
	}
	else if (_pCmd->GetId() == CMD_DYNAMIC_MESSAGE)
	{
		DynamicMessage* pDynamicMessage = _pCmd->CastCommand<DynamicMessage*>();
		Dummy* pDummy = &pDynamicMessage->dmg_;

		if (pDummy->a_ != 1 || pDummy->b_ != 2 || pDummy->c_ != 3 || pDynamicMessage->d_ != 4)
		{
			Console::WriteLine("데이터를 올바르게 수신하지 못했습니다.");
			return;
		}

		int len = pDynamicMessage->count_ - 1;
		Console::WriteLine("[%s] 다이나믹 메시지를 수신했습니다. : %s(길이 : %d)", name_.Source(), pDynamicMessage->Msg(), len);

		MemoryPoolAbstractPtr pMemoryPool;
		auto pPacket2 = dbg_new DynamicCmdPacket<DynamicMessage>(pMemoryPool, len + 2);
		auto pPacket3 = dbg_new DynamicCmdPacket<DynamicMessage>(nullptr, len + 2);
		pPacket2->Release();
		pPacket3->Release();


		// 다이나믹 패킷 에코 진행
		auto pPacket = dbg_new DynamicCmdPacket<DynamicMessage>(len + 1);
		DynamicMessage* pMsg = pPacket->Get<0>();
		StringUtil::CopyUnsafe(pMsg->Msg(), pDynamicMessage->Msg());

		if (!_pReceiver->SendAsync(pPacket))
		{
			Console::WriteLine("[%s] 다이나믹 에코 실패", name_.Source());
			return;
		}

		Console::WriteLine("[%s] 다이나믹 에코", name_.Source());
	}
}


//////////////////////////////////////////////////////////////////////////////////////////
void ServerListener::OnStopped()
{
	Console::WriteLine("[%s] 종료되었습니다.", name_.Source());
}

NS_END
