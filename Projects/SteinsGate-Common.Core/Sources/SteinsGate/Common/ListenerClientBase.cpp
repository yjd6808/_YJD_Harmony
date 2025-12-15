/*
 * 작성자: 윤정도
 * 생성일: 4/5/2023 2:08:27 PM
 * =====================
 *
 */

#include "Core.h"
#include "CommonCoreHeader.h"
#include "ListenerClientBase.h"

#include <SteinsGate/Common/ListenerHelperBase.h>

USING_NS_JC;
USING_NS_JNET;

void ListenerClientBase::OnConnected(SGSession* _pSession)
{
	const IPv4EndPoint& remoteEndPoint = _pSession->GetRemoteEndPoint();
	const String remoteEndPointString = remoteEndPoint.ToString();

	_LogInfo_("%s와 연결되었습니다.", remoteEndPointString.Source());
}

void ListenerClientBase::OnConnectFailed(SGSession* _pSession, Int32U _errorCode)
{
	_LogInfo_("연결 실패 (%d)", _errorCode);
}

void ListenerClientBase::OnDisconnected(SGSession* _pSession, Int32U _errorCode)
{
	const IPv4EndPoint& remoteEndPoint = _pSession->GetRemoteEndPoint();
	const String remoteEndPointString = remoteEndPoint.ToString();

	_LogInfo_("%s와 연결이 종료되었습니다.", remoteEndPointString.Source());
}

void ListenerClientBase::OnSent(SGSession* _pSession, IPacket* _pSentPacket, Int32UL _sentBytes)
{
	const TransportProtocol protocol = _pSession->Protocol();

	if (_pSentPacket->GetType() == PacketType::Command)
	{
		CommandPacket* pPacket = static_cast<CommandPacket*>(_pSentPacket);
		pPacket->ForEach([&](ICommand* cmd) { ListenerHelperBase::LogCommand(protocol, Transmission::Send, cmd); });
	}

	ListenerHelperBase::LogPacketHex(_pSentPacket);
}

void ListenerClientBase::OnReceived(SGSession* _pSession, ICommand* _pRecvCmd)
{
	ListenerHelperBase::LogCommand(_pSession->Protocol(), Transmission::Recv, _pRecvCmd);
}

void ListenerClientBase::OnReceived(Session* _pSession, RecvedCommandPacket* _pRecvPacket)
{
	ListenerHelperBase::LogPacketHex(_pRecvPacket);
}
