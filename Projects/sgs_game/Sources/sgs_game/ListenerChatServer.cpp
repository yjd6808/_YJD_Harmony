/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 9:10:29 AM [SteinsGate-Server.Lobby 프로젝트 복사 생성]
 * =====================
 *
 */

#include "Core.h"
#include "GameCoreHeader.h"
#include "ListenerChatServer.h"

USING_NS_JC;
USING_NS_JNET;

ListenerChatServer::ListenerChatServer(ChatServer* _pServer, CommandParser* _pParser)
	: ListenerServerCommon(_pParser)
	, chatTcp_(_pServer)
{
}

void ListenerChatServer::OnStarted()
{
	ListenerServerCommon::OnStarted();
}

void ListenerChatServer::OnConnected(Session* _pConnectedSession)
{
	ListenerServerCommon::OnConnected(_pConnectedSession);
}

void ListenerChatServer::OnDisconnected(Session* _pDisconnetedSession, Int32U _errorCode)
{
	ListenerServerCommon::OnDisconnected(_pDisconnetedSession, _errorCode);
}

void ListenerChatServer::OnSent(Session* _pSender, IPacket* _pSentPacket, Int32UL _sentBytes)
{
	ListenerServerCommon::OnSent(_pSender, _pSentPacket, _sentBytes);
}

void ListenerChatServer::OnReceived(Session* _pReceiver, ICommand* _pRecvCmd)
{
	ListenerServerCommon::OnReceived(_pReceiver, _pRecvCmd);
}

void ListenerChatServer::OnReceived(Session* _pSession, RecvedCommandPacket* _pRecvPacket)
{
	ListenerServerCommon::OnReceived(_pSession, _pRecvPacket);
}

void ListenerChatServer::OnStopped()
{
	ListenerServerCommon::OnStopped();
}
