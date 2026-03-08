/*
 * 작성자: 윤정도
 * 생성일: 4/5/2023 2:08:27 PM
 * =====================
 *
 */

#include "Core.h"
#include "NetClientListener.h"

#include <jnet/Packet/PacketReader.h>

#include <sg/_Net/NetListenerHelper.h>

USING_NS_JC;
USING_NS_JNET;
USING_NS_SG;

//////////////////////////////////////////////////////////////////////////////////////////
NetClientListener::NetClientListener(jnet::CommandParser* _pParser)
: pParser_(_pParser)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void NetClientListener::OnConnected(jnet::Session* _pSession)
{
	const IPv4EndPoint& remoteEndPoint = _pSession->GetRemoteEndPoint();
	const String remoteEndPointString = remoteEndPoint.ToString();

	_LogInfo_("%s와 연결되었습니다.", remoteEndPointString.Source());

	if (fnConnected_)
	{
		fnConnected_(_pSession);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void NetClientListener::OnConnectFailed(jnet::Session* _pSession, _u32 _errorCode)
{
	_LogInfo_("연결 실패 (%d)", _errorCode);

	if (fnConnectFailed_)
	{
		fnConnectFailed_(_pSession, _errorCode);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void NetClientListener::OnDisconnected(jnet::Session* _pSession, _u32 _errorCode)
{
	const IPv4EndPoint& remoteEndPoint = _pSession->GetRemoteEndPoint();
	const String remoteEndPointString = remoteEndPoint.ToString();

	_LogInfo_("%s와 연결이 종료되었습니다.", remoteEndPointString.Source());

	if (fnDisconnected_)
	{
		fnDisconnected_(_pSession, _errorCode);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void NetClientListener::OnSent(jnet::Session* _pSession, IPacket* _pSentPacket, _u32l _sentBytes)
{
	const TransportProtocol protocol = _pSession->Protocol();

	PacketReader reader(_pSentPacket->GetWSABuf());
	PacketElementView view;
	while (reader.Next(view) == PacketReader::rrSuccess)
	{
		if (view.type_ == static_cast<_u8>(PacketType::Raw))
		{
			NetListenerHelper::LogRaw(protocol, Transmission::Send, view.raw_);
		}
		else if (view.type_ == static_cast<_u8>(PacketType::Command))
		{
			NetListenerHelper::LogCommand(protocol, Transmission::Send, view.cmd_);
		}
		else if (view.type_ == static_cast<_u8>(PacketType::Message))
		{
			NetListenerHelper::LogMessage(protocol, Transmission::Send, view.msg_);
		}
	}

	sg::NetListenerHelper::LogPacketHex(_pSentPacket);

	if (fnSent_)
	{
		fnSent_(_pSession, _pSentPacket, _sentBytes);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void NetClientListener::OnReceivedCmd(jnet::Session* _pSession, ICommand* _pRecvCmd)
{
	sg::NetListenerHelper::LogCommand(_pSession->Protocol(), Transmission::Recv, _pRecvCmd);

	if (pParser_ && !pParser_->RunCommand(_pSession, _pRecvCmd))
	{
		const Cmd_t id = _pRecvCmd->GetId();
		const char* pCommandName = sg::CommandNameDictionary.Get(id);

		_LogWarn_("%c %s %sB %s(%d) Parse Faliled",
			TransmissionName(Transmission::Recv),
			TransportProtocolName(_pSession->Protocol()),
			StringUtil::FillLeft(_pRecvCmd->GetLength(), ' ', 4).Source(),
			pCommandName,
			id);
	}

	if (fnReceivedCmd_)
	{
		fnReceivedCmd_(_pSession, _pRecvCmd);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void NetClientListener::OnReceivedPacket(Session* _pSession, RecvedPacket* _pRecvPacket)
{
	sg::NetListenerHelper::LogPacketHex(_pRecvPacket);

	if (fnReceivedPacket_)
	{
		fnReceivedPacket_(_pSession, _pRecvPacket);
	}
}
