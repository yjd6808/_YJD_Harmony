/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:37:07 PM
 * =====================
 *
 */


#include "Core.h"
#include "_Net/NetServerListener.h"

#include <sg/_Net/NetListenerHelper.h>

USING_NS_JC;
USING_NS_JNET;
USING_NS_SG;

//////////////////////////////////////////////////////////////////////////////////////////
NetServerListener::NetServerListener(jnet::TcpServer* _pServer, jnet::CommandParser* _pParser)
: pServer_(_pServer)
, pParser_(_pParser)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void NetServerListener::OnStarted()
{
	const char* pServerName = pServer_->GetName();
	_LogInfo_("%s 서버가 시작되었습니다.", pServerName);
}

//////////////////////////////////////////////////////////////////////////////////////////
void NetServerListener::OnConnected(Session* _pConnectedSession)
{
	const IPv4EndPoint& remoteEndPoint = _pConnectedSession->GetRemoteEndPoint();
	const String remoteEndPointString = remoteEndPoint.ToString();

	_LogInfo_("%s 클라이언트 접속", remoteEndPointString.Source());
}

//////////////////////////////////////////////////////////////////////////////////////////
void NetServerListener::OnDisconnected(Session* _pDisconnectedSession, Int32U _errorCode)
{
	const IPv4EndPoint& remoteEndPoint = _pDisconnectedSession->GetRemoteEndPoint();
	const String remoteEndPointString = remoteEndPoint.ToString();

	_LogInfo_("%s 클라이언트 연결종료", remoteEndPointString.Source());
}

//////////////////////////////////////////////////////////////////////////////////////////
void NetServerListener::OnSent(Session* _pSender, IPacket* _pSentPacket, Int32UL _sentBytes)
{
	if (_pSentPacket->GetType() == PacketType::Command)
	{
		CommandPacket* pPacket = static_cast<CommandPacket*>(_pSentPacket);
		pPacket->ForEach([&](ICommand* _pCmd)
		{
			NetListenerHelper::LogCommand(_pSender->Protocol(), Transmission::Send, _pCmd);
		});
	}

	NetListenerHelper::LogPacketHex(_pSentPacket);
}

//////////////////////////////////////////////////////////////////////////////////////////
void NetServerListener::OnReceived(Session* _pReceiver, ICommand* _pRecvCmd)
{
	NetListenerHelper::LogCommand(_pReceiver->Protocol(), Transmission::Recv, _pRecvCmd);

	if (pParser_ && !pParser_->RunCommand(_pReceiver, _pRecvCmd))
	{
		const Cmd_t id = _pRecvCmd->GetId();
		const char* pCommandName = sg::CommandNameDictionary.Get(id);

		_LogWarn_("%c %s %sB %s(%d) Parse Faliled",
			TransmissionName(Transmission::Recv),
			TransportProtocolName(_pReceiver->Protocol()),
			StringUtil::FillLeft(_pRecvCmd->GetLength(), ' ', 4).Source(),
			pCommandName,
			id);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void NetServerListener::OnReceived(Session* _pReceiver, RecvedCommandPacket* _pRecvPacket)
{
	NetListenerHelper::LogPacketHex(_pRecvPacket);
}

//////////////////////////////////////////////////////////////////////////////////////////
void NetServerListener::OnStopped()
{
	_LogInfo_("%s 서버가 중지되었습니다.", pServer_->GetName());
}
