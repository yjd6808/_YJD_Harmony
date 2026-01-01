/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 8:34:29 AM [SteinsGate-Server.Auth 프로젝트 복사 생성]
 * =====================
 *
 */

#include "Core.h"
#include "LobbyCoreHeader.h"
#include "ListenerLobbyServer.h"

#include <jnet/Packet/PacketViewer.h>

#include <sgs/QueryDefine.h>
#include <sgs_lobby/R_LOBBY.h>

USING_NS_JC;
USING_NS_JNET;

//////////////////////////////////////////////////////////////////////////////////////////
ListenerLobbyServer::ListenerLobbyServer(LobbyServer* _pServer, CommandParser* _pParser)
: ListenerServerCommon(_pParser)
, lobbyTcp_(_pServer)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void ListenerLobbyServer::OnStarted()
{
	ListenerServerCommon::OnStarted();
}

//////////////////////////////////////////////////////////////////////////////////////////
void ListenerLobbyServer::OnConnected(Session* _pConnectedSession)
{
	ListenerServerCommon::OnConnected(_pConnectedSession);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ListenerLobbyServer::OnDisconnected(Session* _pDisconnectedSession, Int32U _errorCode)
{
	ListenerServerCommon::OnDisconnected(_pDisconnectedSession, _errorCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ListenerLobbyServer::OnSent(Session* _pSender, IPacket* _pSentPacket, Int32UL _sentBytes)
{
	ListenerServerCommon::OnSent(_pSender, _pSentPacket, _sentBytes);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ListenerLobbyServer::OnReceived(Session* _pReceiver, ICommand* _pRecvCmd)
{
	ListenerServerCommon::OnReceived(_pReceiver, _pRecvCmd);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ListenerLobbyServer::OnReceived(Session* _pSession, RecvedCommandPacket* _pRecvPacket)
{
	ListenerServerCommon::OnReceived(_pSession, _pRecvPacket);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ListenerLobbyServer::OnStopped()
{
	ListenerServerCommon::OnStopped();
}
