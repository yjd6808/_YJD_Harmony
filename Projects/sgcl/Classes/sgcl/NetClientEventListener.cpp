/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:37:07 PM
 * =====================
 * IOCPThreadAccessPacketQueue는 IOCP 쓰레드에서 수신한 패킷을 복사한 패킷을 담기위한 용도의 큐이다.
 */


#include "Core.h"
#include "NetClientEventListener.h"

#include <sgcl/NetCore.h>
#include <sgcl/CommandSynchronizer.h>
#include <sgcl/ConnectionSynchronizer.h>

USING_NS_JC;
USING_NS_CC;
USING_NS_JNET;

//////////////////////////////////////////////////////////////////////////////////////////
NetClientEventListener::NetClientEventListener(ClientConnectServerType_t _connectedServerType)
: connectedServerType_(_connectedServerType)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void NetClientEventListener::OnConnected(Session* _pSession)
{
	ListenerClientBase::OnConnected(_pSession);
	SyncConnectionResult(connectedServerType_, _pSession, true, 0);
}

//////////////////////////////////////////////////////////////////////////////////////////
void NetClientEventListener::OnDisconnected(Session* _pSession, Int32U _errorCode)
{
	ListenerClientBase::OnDisconnected(_pSession, _errorCode);
	SyncDisconnectionResult(connectedServerType_, _pSession);
}

//////////////////////////////////////////////////////////////////////////////////////////
void NetClientEventListener::OnConnectFailed(Session* _pSession, Int32U _errorCode)
{
	ListenerClientBase::OnConnectFailed(_pSession, _errorCode);
	SyncConnectionResult(connectedServerType_, _pSession, false, _errorCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
void NetClientEventListener::OnSent(Session* _pSession, IPacket* _pSendPacket, Int32UL _sentBytes)
{
	ListenerClientBase::OnSent(_pSession, _pSendPacket, _sentBytes);
}

//////////////////////////////////////////////////////////////////////////////////////////
void NetClientEventListener::OnReceived(Session* _pSession, ICommand* _pRecvCmd)
{
	ListenerClientBase::OnReceived(_pSession, _pRecvCmd);
	SyncReceivedCommand(connectedServerType_, _pSession, _pRecvCmd);
}

//////////////////////////////////////////////////////////////////////////////////////////
void NetClientEventListener::OnReceived(Session* _pSession, RecvedCommandPacket* _pRecvPacket)
{
	ListenerClientBase::OnReceived(_pSession, _pRecvPacket);
}

//////////////////////////////////////////////////////////////////////////////////////////
void NetClientEventListener::SyncConnectionResult(ClientConnectServerType_t _listenerType, Session* _pSession,
                                                  bool _success, Int32U _errorCode)
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
void NetClientEventListener::SyncDisconnectionResult(ClientConnectServerType_t _listenerType, Session* _pSession)
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
void NetClientEventListener::SyncReceivedCommand(ClientConnectServerType_t _listenerType, jnet::Session* _pSession,
                                                 ICommand* _pCmd)
{
	CommandSynchronizer* pSynchronizer = g_cNet.GetCommandSynchronizer();

	if (pSynchronizer == nullptr)
	{
		_LogWarn_("커맨드를 수신했지만 동기화기가 소멸되어서 이를 메인쓰레드에서 받아서 처리할 수가 없습니다. (커맨드:%d)", _pCmd->GetId());
		return;
	}

	pSynchronizer->EnqueueCommand(_listenerType, _pSession, _pCmd);
}
