/*
 * 작성자: 윤정도
 * 생성일: 4/5/2023 2:08:27 PM
 * =====================
 *
 */

#include "Core.h"
#include "NetClientListener_InterServ.h"

#include <jnet/Packet/SendHelper.h>

#include <sgs/S_SETUP_IS_COMMON.h>
#include <sgs/R_SETUP_COMMON.h>

USING_NS_JC;
USING_NS_JNET;
USING_NS_SG;

//////////////////////////////////////////////////////////////////////////////////////////
NetClientListener_InterServ::NetClientListener_InterServ(ServerProcessType_t _serverProcessType, CommandParser* _pParser)
: NetClientListener(_pParser)
, serverProcessType_(_serverProcessType)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void NetClientListener_InterServ::OnConnected(Session* _pSession)
{
	NetClientListener::OnConnected(_pSession);

	S_SETUP_IS_COMMON::SetInformation(_pSession, SendStrategy::SendAsync, SingleServerType::Center);
	S_SETUP_IS_COMMON::SEND_SCE_ItsMe(serverProcessType_);
}

//////////////////////////////////////////////////////////////////////////////////////////
void NetClientListener_InterServ::OnDisconnected(Session* _pSession, Int32U _errorCode)
{
	NetClientListener::OnDisconnected(_pSession, _errorCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
void NetClientListener_InterServ::OnSent(Session* _pSession, IPacket* _pSentPacket, Int32UL _sentBytes)
{
	NetClientListener::OnSent(_pSession, _pSentPacket, _sentBytes);
}

//////////////////////////////////////////////////////////////////////////////////////////
void NetClientListener_InterServ::OnReceived(Session* _pSession, ICommand* _pRecvCmd)
{
	NetClientListener::OnReceived(_pSession, _pRecvCmd);
}

//////////////////////////////////////////////////////////////////////////////////////////
void NetClientListener_InterServ::OnReceived(Session* _pSession, RecvedCommandPacket* _pRecvPacket)
{
	NetClientListener::OnReceived(_pSession, _pRecvPacket);
}

//////////////////////////////////////////////////////////////////////////////////////////
void NetClientListener_InterServ::OnConnectFailed(Session* _pSession, Int32U _errorCode)
{
	NetClientListener::OnConnectFailed(_pSession, _errorCode);
}
