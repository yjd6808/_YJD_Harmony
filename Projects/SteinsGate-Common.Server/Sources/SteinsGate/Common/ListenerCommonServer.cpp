/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:37:07 PM
 * =====================
 *
 */


#include "Server.h"
#include "ServerCoreHeader.h"
#include "ListenerCommonServer.h"

USING_NS_JC;
USING_NS_JNET;

void ListenerCommonServer::OnStarted() {
	_LogInfo_("%s 서버가 시작되었습니다.", GetServerName());
}

void ListenerCommonServer::OnConnected(Session* connectedSession) {
	const IPv4EndPoint& remoteEndPoint = connectedSession->GetRemoteEndPoint();
	const String remoteEndPointString = remoteEndPoint.ToString();

	_LogInfo_("%s 클라이언트 접속", remoteEndPointString.Source());
}

void ListenerCommonServer::OnDisconnected(Session* disconnetedSession) {
	const IPv4EndPoint& remoteEndPoint = disconnetedSession->GetRemoteEndPoint();
	const String remoteEndPointString = remoteEndPoint.ToString();

	_LogInfo_("%s 클라이언트 연결종료", remoteEndPointString.Source());
}

void ListenerCommonServer::OnSent(Session* sender, ISendPacket* sentPacket, Int32UL sentBytes) {
}

void ListenerCommonServer::OnReceived(Session* receiver, ICommand* cmd) {
	if (!Parser.RunCommand(receiver, cmd)) {
		_LogWarn_("커맨드: %d 수행 실패 (%s)", cmd->GetCommand());
		return;
	}

	_LogDebug_("%s 실행완료", Parser.GetCommandName(cmd->GetCommand()), GetServerName());
}

void ListenerCommonServer::OnStopped() {
	_LogInfo_("%s 서버가 중지되었습니다.", GetServerName());
}


