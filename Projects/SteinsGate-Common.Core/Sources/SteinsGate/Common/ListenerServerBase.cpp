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

void ListenerServerBase::OnStarted() {
	const char* z = GetServerName();
	_LogInfo_("%s 서버가 시작되었습니다.", GetServerName());
}

void ListenerServerBase::OnConnected(Session* connectedSession) {
	const IPv4EndPoint& remoteEndPoint = connectedSession->GetRemoteEndPoint();
	const String remoteEndPointString = remoteEndPoint.ToString();

	_LogInfo_("%s 클라이언트 접속", remoteEndPointString.Source());
}

void ListenerServerBase::OnDisconnected(Session* disconnetedSession, Int32U errorCode) {
	const IPv4EndPoint& remoteEndPoint = disconnetedSession->GetRemoteEndPoint();
	const String remoteEndPointString = remoteEndPoint.ToString();

	_LogInfo_("%s 클라이언트 연결종료", remoteEndPointString.Source());
}

void ListenerServerBase::OnSent(Session* sender, IPacket* sentPacket, Int32UL sentBytes) {
	if (sentPacket->GetType() == PacketType::Command) {
		CommandPacket* pPacket = static_cast<CommandPacket*>(sentPacket);
		pPacket->ForEach([&](ICommand* cmd) { ListenerHelperBase::LogCommand(sender->Protocol(), Transmission::Send, cmd); });
	}
	ListenerHelperBase::LogPacketHex(sentPacket);
}

void ListenerServerBase::OnReceived(Session* receiver, ICommand* cmd) {
	ListenerHelperBase::LogCommand(receiver->Protocol(), Transmission::Recv, cmd);
}

void ListenerServerBase::OnReceived(Session* receiver, RecvedCommandPacket* recvPacket) {
	ListenerHelperBase::LogPacketHex(recvPacket);
}

void ListenerServerBase::OnStopped() {
	_LogInfo_("%s 서버가 중지되었습니다.", GetServerName());
}


