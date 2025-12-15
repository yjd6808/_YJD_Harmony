/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:37:07 PM
 * =====================
 *
 */


#include "Core.h"
#include "CommonCoreHeader.h"
#include "ListenerServerBase.h"

#include <SteinsGate/Common/ListenerHelperBase.h>

USING_NS_JC;
USING_NS_JNET;

//////////////////////////////////////////////////////////////////////////////////////////
void ListenerServerBase::OnStarted()
{
	const char* pServerName = GetServerName();
	_LogInfo_("%s 서버가 시작되었습니다.", pServerName);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ListenerServerBase::OnConnected(Session* _pConnectedSession)
{
	const IPv4EndPoint& remoteEndPoint = _pConnectedSession->GetRemoteEndPoint();
	const String remoteEndPointString = remoteEndPoint.ToString();

	_LogInfo_("%s 클라이언트 접속", remoteEndPointString.Source());
}

//////////////////////////////////////////////////////////////////////////////////////////
void ListenerServerBase::OnDisconnected(Session* _pDisconnectedSession, Int32U _errorCode)
{
	const IPv4EndPoint& remoteEndPoint = _pDisconnectedSession->GetRemoteEndPoint();
	const String remoteEndPointString = remoteEndPoint.ToString();

	_LogInfo_("%s 클라이언트 연결종료", remoteEndPointString.Source());
}

//////////////////////////////////////////////////////////////////////////////////////////
void ListenerServerBase::OnSent(Session* _pSender, IPacket* _pSentPacket, Int32UL _sentBytes)
{
	if (_pSentPacket->GetType() == PacketType::Command)
	{
		CommandPacket* pPacket = static_cast<CommandPacket*>(_pSentPacket);
		pPacket->ForEach([&](ICommand* _pCmd)
		{
			ListenerHelperBase::LogCommand(_pSender->Protocol(), Transmission::Send, _pCmd);
		});
	}

	ListenerHelperBase::LogPacketHex(_pSentPacket);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ListenerServerBase::OnReceived(Session* _pReceiver, ICommand* _pRecvCmd)
{
	ListenerHelperBase::LogCommand(_pReceiver->Protocol(), Transmission::Recv, _pRecvCmd);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ListenerServerBase::OnReceived(Session* _pReceiver, RecvedCommandPacket* _pRecvPacket)
{
	ListenerHelperBase::LogPacketHex(_pRecvPacket);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ListenerServerBase::OnStopped()
{
	_LogInfo_("%s 서버가 중지되었습니다.", GetServerName());
}
