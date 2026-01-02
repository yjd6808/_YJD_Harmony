/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:37:07 PM
 * =====================
 *
 */


#include "Core.h"
#include "AuthCoreHeader.h"
#include "ListenerAuthServer.h"

USING_NS_JC;
USING_NS_JNET;

//////////////////////////////////////////////////////////////////////////////////////////
ListenerAuthServer::ListenerAuthServer(AuthServer* _pServer, CommandParser* _pParser)
	: ListenerServerCommon(_pParser)
	, authTcp_(_pServer)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void ListenerAuthServer::OnStarted()
{
	ListenerServerCommon::OnStarted();
}

//////////////////////////////////////////////////////////////////////////////////////////
void ListenerAuthServer::OnConnected(Session* _pConnectedSession)
{
	ListenerServerCommon::OnConnected(_pConnectedSession);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ListenerAuthServer::OnDisconnected(Session* _pDisconnectedSession, Int32U _errorCode)
{
	ListenerServerCommon::OnDisconnected(_pDisconnectedSession, _errorCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ListenerAuthServer::OnSent(Session* _pSender, IPacket* _pSentPacket, Int32UL _sentBytes)
{
	ListenerServerCommon::OnSent(_pSender, _pSentPacket, _sentBytes);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ListenerAuthServer::OnReceived(Session* _pReceiver, ICommand* _pRecvCmd)
{
	ListenerServerCommon::OnReceived(_pReceiver, _pRecvCmd);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ListenerAuthServer::OnStopped()
{
	ListenerServerCommon::OnStopped();
}
