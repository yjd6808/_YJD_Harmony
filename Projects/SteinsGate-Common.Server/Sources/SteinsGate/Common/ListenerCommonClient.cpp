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

void ListenerCommonClient::OnSent(SGSession* session, ISendPacket* sentPacket, Int32UL sentBytes) {
	int iCmdCount = sentPacket->GetCommandCount();
	int iPos = 0;

	String szSentText(512);
	String szSentPackets(256);

	/*
	sentPacket->ForEach([&](ICommand* cmd) {
		const char* szDelim = iPos < iCmdCount - 1 ? ", " : "";
		const Cmd_t id = cmd->GetCommand();
		szSentPackets += StringUtil::Format("%s(%d)%s", CoreCommandNameDictionary_v.Get(id), id, szDelim);
		++iPos;
	});
	szSentText += StringUtil::Format("[S][%d %s][%d B][%s]", iCmdCount, TransportProtocolName(session->Protocol()), sentBytes, szSentPackets.Source());
	_LogInfo_(szSentText.Source());
	*/

	sentPacket->ForEach([&](ICommand* cmd) {
		const Cmd_t id = cmd->GetCommand();
		_LogInfo_("S %s %sB %s(%d)", TransportProtocolName(session->Protocol()), StringUtil::FillLeft(cmd->CmdLen, ' ', 4).Source(), CoreCommandNameDictionary_v.Get(id), id);
		++iPos;
	});
	
}

void ListenerCommonClient::OnReceived(SGSession* session, ICommand* cmd) {
	const Cmd_t id = cmd->GetCommand();
	const char* szName = CoreCommandNameDictionary_v.Get(id);

	if (!Parser.RunCommand(session, cmd)) {
		_LogWarn_("R %s %sB %s(%d) - Failed", TransportProtocolName(session->Protocol()), StringUtil::FillLeft(cmd->CmdLen, ' ', 4).Source(), szName, id);
		return;
	}

	_LogInfo_("R %s %sB %s(%d)", TransportProtocolName(session->Protocol()), StringUtil::FillLeft(cmd->CmdLen, ' ', 4).Source(), szName, id);
}

