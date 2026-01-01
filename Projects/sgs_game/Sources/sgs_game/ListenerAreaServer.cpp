/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 9:10:29 AM [SteinsGate-Server.Lobby 프로젝트 복사 생성]
 * =====================
 *
 */


#include "Core.h"
#include "GameCoreHeader.h"
#include "ListenerAreaServer.h"

USING_NS_JC;
USING_NS_JNET;

ListenerAreaServer::ListenerAreaServer(AreaServer* _pServer, CommandParser* _pParser)
	: ListenerServerCommon(_pParser)
	, areaTcp_(_pServer)
{
}

void ListenerAreaServer::OnStarted()
{
	ListenerServerCommon::OnStarted();
}

void ListenerAreaServer::OnConnected(Session* _pConnectedSession)
{
	ListenerServerCommon::OnConnected(_pConnectedSession);
}

void ListenerAreaServer::OnDisconnected(Session* _pDisconnectedSession, Int32U _errorCode)
{
	ListenerServerCommon::OnDisconnected(_pDisconnectedSession, _errorCode);
}

void ListenerAreaServer::OnSent(Session* _pSender, IPacket* _pSentPacket, Int32UL _sentBytes)
{
	ListenerServerCommon::OnSent(_pSender, _pSentPacket, _sentBytes);
}

void ListenerAreaServer::OnReceived(Session* _pReceiver, ICommand* _pCmd)
{
	ListenerServerCommon::OnReceived(_pReceiver, _pCmd);
}

void ListenerAreaServer::OnReceived(Session* _pSession, RecvedCommandPacket* _pRecvPacket)
{
	ListenerServerCommon::OnReceived(_pSession, _pRecvPacket);
}

void ListenerAreaServer::OnStopped()
{
	ListenerServerCommon::OnStopped();
}

