/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:37:07 PM
 * =====================
 *
 */


#include "Server.h"
#include "ServerCoreHeader.h"
#include "ListenerCommonServer.h"

#include "JCore/Logger/ConsoleLogger.h"

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
	int iCmdCount = sentPacket->GetCommandCount();
	int iPos = 0;

	/*
	String szSentText(512);
	String szSentPackets(256);

	sentPacket->ForEach([&](ICommand* cmd) {
		const char* szDelim = iPos < iCmdCount - 1 ? ", " : "";
		const Cmd_t id = cmd->GetCommand();
		szSentPackets += StringUtil::Format("%s(%d)%s", CoreCommandNameDictionary_v.Get(id), id, szDelim);
		++iPos;
	});
	szSentText += StringUtil::Format("[S][%d %s][%d B][%s]", iCmdCount, TransportProtocolName(sender->Protocol()), sentBytes, szSentPackets.Source());
	*/

	sentPacket->ForEach([&](ICommand* cmd) {
		const Cmd_t id = cmd->GetCommand();
		_LogInfo_("S %s %sB %s(%d)", TransportProtocolName(sender->Protocol()), StringUtil::FillLeft(cmd->CmdLen, ' ', 4).Source(), CoreCommandNameDictionary_v.Get(id), id);
		++iPos;
	});
}

void ListenerCommonServer::OnReceived(Session* receiver, ICommand* cmd) {
	const Cmd_t id = cmd->GetCommand();
	const char* szName = CoreCommandNameDictionary_v.Get(id);

	if (!Parser.RunCommand(receiver, cmd)) {
		_LogInfo_("R %s %sB %s(%d) - Failed", TransportProtocolName(receiver->Protocol()), StringUtil::FillLeft(cmd->CmdLen, ' ', 4).Source(), szName, id);
		return;
	}


	_LogInfo_("R %s %sB %s(%d)", TransportProtocolName(receiver->Protocol()), StringUtil::FillLeft(cmd->CmdLen, ' ', 4).Source(), szName, id);
}

void ListenerCommonServer::OnStopped() {
	_LogInfo_("%s 서버가 중지되었습니다.", GetServerName());
}


