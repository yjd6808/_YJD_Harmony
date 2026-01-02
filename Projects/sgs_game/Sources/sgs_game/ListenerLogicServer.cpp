/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 9:10:29 AM [SteinsGate-Server.Lobby 프로젝트 복사 생성]
 * =====================
 *
 */

#include "Core.h"
#include "GameCoreHeader.h"
#include "ListenerLogicServer.h"

USING_NS_JC;
USING_NS_JNET;

//////////////////////////////////////////////////////////////////////////////////////////
ListenerLogicServer::ListenerLogicServer(LogicServer* _pLogicServer, CommandParser* _pParser)
	: ListenerServerCommon(_pParser)
	, logicTcp_(_pLogicServer)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void ListenerLogicServer::OnStarted()
{
	ListenerServerCommon::OnStarted();
}

//////////////////////////////////////////////////////////////////////////////////////////
void ListenerLogicServer::OnConnected(Session* _pConnectedSession)
{
	ListenerServerCommon::OnConnected(_pConnectedSession);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ListenerLogicServer::OnDisconnected(Session* _pDisconnectedSession, Int32U _errorCode)
{
	ListenerServerCommon::OnDisconnected(_pDisconnectedSession, _errorCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ListenerLogicServer::OnSent(Session* _pSender, IPacket* _pSentPacket, Int32UL _sentBytes)
{
	ListenerServerCommon::OnSent(_pSender, _pSentPacket, _sentBytes);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ListenerLogicServer::OnReceived(Session* _pReceiver, ICommand* _pCmd)
{
	ListenerServerCommon::OnReceived(_pReceiver, _pCmd);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ListenerLogicServer::OnReceived(Session* _pSession, RecvedCommandPacket* _pRecvPacket)
{
	ListenerServerCommon::OnReceived(_pSession, _pRecvPacket);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ListenerLogicServer::OnStopped()
{
	ListenerServerCommon::OnStopped();
}
