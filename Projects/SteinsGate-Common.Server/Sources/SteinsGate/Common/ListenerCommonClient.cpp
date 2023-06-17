/*
 * 작성자: 윤정도
 * 생성일: 4/5/2023 2:08:27 PM
 * =====================
 *
 */

#include "Server.h"
#include "ServerCoreHeader.h"
#include "ListenerCommonClient.h"

USING_NS_JC;
USING_NS_JNET;

void ListenerCommonClient::OnConnected(SGSession* session) {
	const IPv4EndPoint& remoteEndPoint = session->GetRemoteEndPoint();
	const String remoteEndPointString = remoteEndPoint.ToString();

	_LogInfo_("%s와 연결되었습니다.", remoteEndPointString.Source());
}

void ListenerCommonClient::OnConnectFailed(SGSession* session, Int32U errorCode) {
	_LogInfo_("연결 실패 (%d)", errorCode);
}

void ListenerCommonClient::OnDisconnected(SGSession* session) {
	const IPv4EndPoint& remoteEndPoint = session->GetRemoteEndPoint();
	const String remoteEndPointString = remoteEndPoint.ToString();

	_LogInfo_("%s와 연결이 종료되었습니다.", remoteEndPointString.Source());
}

void ListenerCommonClient::OnSent(SGSession* session, JNetwork::ISendPacket* sentPacket, Int32UL sentBytes) {
	int iMax = sentPacket->GetCommandCount();
	int iPos = 0;

	_LogInfo_("%d개 %s 커맨드 송신 (%d 바이트)", iMax, JNetwork::TransportProtocolName(session->Protocol()), sentBytes);
}

void ListenerCommonClient::OnReceived(SGSession* session, JNetwork::ICommand* cmd) {
	if (!Parser.RunCommand(session, cmd)) {
		_LogWarn_("커맨드: %d 수행 실패", cmd->GetCommand());
		return;
	}

	_LogDebug_("커맨드: %s 실행완료", Parser.GetCommandName(cmd->GetCommand()));
}

