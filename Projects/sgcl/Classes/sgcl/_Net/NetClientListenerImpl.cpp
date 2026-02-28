/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:37:07 PM
 * =====================
 * IOCPThreadAccessPacketQueue는 IOCP 쓰레드에서 수신한 패킷을 복사한 패킷을 담기위한 용도의 큐이다.
 */


#include "Core.h"
#include "NetClientListenerImpl.h"

#include <sgcl/_Net/NetCore.h>
#include <sgcl/_Net/CommandSynchronizer.h>
#include <sgcl/_Net/ConnectionSynchronizer.h>

USING_NS_JC;
USING_NS_CC;
USING_NS_JNET;

static void SyncConnectionResult(ServerType_t _listenerType, jnet::Session* _pSession, bool _success, _u32 _errorCode);
static void SyncDisconnectionResult(ServerType_t _listenerType, jnet::Session* _pSession);
static void SyncReceivedCommand(ServerType_t _listenerType, jnet::Session* _pSession, jnet::ICommand* _pCmd);

//////////////////////////////////////////////////////////////////////////////////////////
NetClientListenerImpl::NetClientListenerImpl(ServerType_t _connectedServerType)
: NetClientListener(nullptr)
, connectedServerType_(_connectedServerType)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void NetClientListenerImpl::OnConnected(Session* _pSession)
{
	NetClientListener::OnConnected(_pSession);
	SyncConnectionResult(connectedServerType_, _pSession, true, 0);
}

//////////////////////////////////////////////////////////////////////////////////////////
void NetClientListenerImpl::OnDisconnected(Session* _pSession, _u32 _errorCode)
{
	NetClientListener::OnDisconnected(_pSession, _errorCode);
	SyncDisconnectionResult(connectedServerType_, _pSession);
}

//////////////////////////////////////////////////////////////////////////////////////////
void NetClientListenerImpl::OnConnectFailed(Session* _pSession, _u32 _errorCode)
{
	NetClientListener::OnConnectFailed(_pSession, _errorCode);
	SyncConnectionResult(connectedServerType_, _pSession, false, _errorCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
void NetClientListenerImpl::OnSent(Session* _pSession, IPacket* _pSendPacket, _u32l _sentBytes)
{
	NetClientListener::OnSent(_pSession, _pSendPacket, _sentBytes);
}

//////////////////////////////////////////////////////////////////////////////////////////
void NetClientListenerImpl::OnReceived(Session* _pSession, ICommand* _pRecvCmd)
{
	NetClientListener::OnReceived(_pSession, _pRecvCmd);
	SyncReceivedCommand(connectedServerType_, _pSession, _pRecvCmd);
}

//////////////////////////////////////////////////////////////////////////////////////////
void NetClientListenerImpl::OnReceived(Session* _pSession, RecvedCmdPacket* _pRecvPacket)
{
	NetClientListener::OnReceived(_pSession, _pRecvPacket);
}

//////////////////////////////////////////////////////////////////////////////////////////
void SyncConnectionResult(
	ServerType_t _listenerType, 
	Session* _pSession,
	bool _success, 
	_u32 _errorCode)
//////////////////////////////////////////////////////////////////////////////////////////
{
	ConnectionSynchronizer* pSynchronizer = g_cNet.GetConnectionSynchronizer();

	if (pSynchronizer == nullptr)
	{
		jc::String szResult = StringUtil::ToString(_success);
		_LogWarn_("동기화기가 소멸되어서 Connection의 후속처리를 수행할 수 없습니다. [타입:%d][결과:%s]", _listenerType, szResult.Source());
		return;
	}

	pSynchronizer->EnqueueConnection(_listenerType, _pSession, _success, _errorCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
void SyncDisconnectionResult(ServerType_t _listenerType, Session* _pSession)
{
	ConnectionSynchronizer* pSynchronizer = g_cNet.GetConnectionSynchronizer();

	if (pSynchronizer == nullptr)
	{
		_LogWarn_("동기화기가 소멸되어서 Disconnection의 후속처리를 수행할 수 없습니다. [타입:%d]", _listenerType);
		return;
	}

	pSynchronizer->EnqueueDisconnection(_listenerType, _pSession);
}

//////////////////////////////////////////////////////////////////////////////////////////
void SyncReceivedCommand(ServerType_t _listenerType, jnet::Session* _pSession, ICommand* _pCmd)
{
	CommandSynchronizer* pSynchronizer = g_cNet.GetCommandSynchronizer();

	if (pSynchronizer == nullptr)
	{
		_LogWarn_("커맨드를 수신했지만 동기화기가 소멸되어서 이를 메인쓰레드에서 받아서 처리할 수가 없습니다. (커맨드:%d)", _pCmd->GetId());
		return;
	}

	pSynchronizer->EnqueueCommand(_listenerType, _pSession, _pCmd);
}
