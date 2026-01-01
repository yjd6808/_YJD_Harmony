/*
 * 작성자: 윤정도
 * 생성일: 4/5/2023 2:08:27 PM
 * =====================
 *
 */


#include "Core.h"
#include "ServerCoreHeader.h"
#include "ListenerInterServerClient.h"

#include <jnet/Packet/SendHelper.h>

#include <sgs/CmdHost.h>
#include <sgs/CmdRelay.h>

#include <sgs/S_SETUP_IS_COMMON.h>
#include <sgs/R_SETUP_COMMON.h>

USING_NS_JC;
USING_NS_JNET;

//////////////////////////////////////////////////////////////////////////////////////////
ListenerInterServerClient::ListenerInterServerClient(ServerProcessType_t _serverProcessType, CommandParser* _pParser)
: ListenerClientCommon(_pParser)
, serverProcessType_(_serverProcessType)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void ListenerInterServerClient::OnConnected(Session* _pSession)
{
	ListenerClientCommon::OnConnected(_pSession);

	S_SETUP_IS_COMMON::SetInformation(sg::InterServerClientTcp, SendStrategy::SendAsync, SingleServerType::Center);
	S_SETUP_IS_COMMON::SEND_SCE_ItsMe(serverProcessType_, sg::ServerProcessInfo->serverId_);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ListenerInterServerClient::OnDisconnected(Session* _pSession, Int32U _errorCode)
{
	ListenerClientCommon::OnDisconnected(_pSession, _errorCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ListenerInterServerClient::OnSent(Session* _pSession, IPacket* _pSentPacket, Int32UL _sentBytes)
{
	ListenerClientCommon::OnSent(_pSession, _pSentPacket, _sentBytes);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ListenerInterServerClient::OnReceived(Session* _pSession, ICommand* _pRecvCmd)
{
	ListenerClientCommon::OnReceived(_pSession, _pRecvCmd);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ListenerInterServerClient::OnReceived(Session* _pSession, RecvedCommandPacket* _pRecvPacket)
{
	ListenerClientCommon::OnReceived(_pSession, _pRecvPacket);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ListenerInterServerClient::OnConnectFailed(Session* _pSession, Int32U _errorCode)
{
	ListenerClientCommon::OnConnectFailed(_pSession, _errorCode);
}
