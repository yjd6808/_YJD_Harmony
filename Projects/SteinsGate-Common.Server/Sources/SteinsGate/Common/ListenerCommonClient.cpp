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
	int iCmdCount = sentPacket->GetCommandCount();
	int iPos = 0;

	String szSentText(512);
	szSentText += StringUtil::Format("%s 커맨드 %d개 송신 (%d B)\n", JNetwork::TransportProtocolName(session->Protocol()), iCmdCount, sentBytes);
	sentPacket->ForEach([&](ICommand* cmd) {
		const char* szBreakLine = iPos < iCmdCount - 1 ? "\n" : "";
		const Cmd_t id = cmd->GetCommand();
		szSentText += StringUtil::Format(" - %s(%d)%s", CoreCommandNameDictionary_v.Get(id), id, szBreakLine);
		++iPos;
	});

	_LogInfo_(szSentText.Source());
}

void ListenerCommonClient::OnReceived(SGSession* session, JNetwork::ICommand* cmd) {
	const Cmd_t id = cmd->GetCommand();

	if (!Parser.RunCommand(session, cmd)) {
		_LogWarn_("커맨드: %s(%d) 수행 실패", CoreCommandNameDictionary_v.Get(id), id);
		return;
	}

	_LogDebug_("커맨드: %s(%d) 실행완료", CoreCommandNameDictionary_v.Get(id), id);
}

