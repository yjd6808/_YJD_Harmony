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

void ListenerClientBase::OnConnected(SGSession* session) {
	const IPv4EndPoint& remoteEndPoint = session->GetRemoteEndPoint();
	const String remoteEndPointString = remoteEndPoint.ToString();

	_LogInfo_("%s와 연결되었습니다.", remoteEndPointString.Source());
}

void ListenerClientBase::OnConnectFailed(SGSession* session, Int32U errorCode) {
	_LogInfo_("연결 실패 (%d)", errorCode);
}

void ListenerClientBase::OnDisconnected(SGSession* session) {
	const IPv4EndPoint& remoteEndPoint = session->GetRemoteEndPoint();
	const String remoteEndPointString = remoteEndPoint.ToString();

	_LogInfo_("%s와 연결이 종료되었습니다.", remoteEndPointString.Source());
}

void ListenerClientBase::OnSent(SGSession* session, ISendPacket* sentPacket, Int32UL sentBytes) {
	const TransportProtocol protocol = session->Protocol();
	sentPacket->ForEach([&](ICommand* cmd) { ListenerHelperBase::LogCommand(protocol, Transmission::Send, cmd); });
	ListenerHelperBase::LogPacketHex(sentPacket);
}

void ListenerClientBase::OnReceived(SGSession* session, ICommand* recvCmd) {
	ListenerHelperBase::LogCommand(session->Protocol(), Transmission::Recv, recvCmd);
}

void ListenerClientBase::OnReceived(Session* session, IRecvPacket* recvPacket) {
	ListenerHelperBase::LogPacketHex(recvPacket);
}

