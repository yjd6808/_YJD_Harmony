/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:37:07 PM
 * =====================
 * IOCPThreadAccessPacketQueue는 IOCP 쓰레드에서 수신한 패킷을 복사한 패킷을 담기위한 용도의 큐이다.
 */


#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "NetClientEventListener.h"

#include <SteinsGate/Client/CommandSynchronizer.h>
#include <SteinsGate/Client/ConnectionSynchronizer.h>

USING_NS_JC;
USING_NS_CC;
USING_NS_JNET;

NetClientEventListener::NetClientEventListener(ClientConnectServerType_t m_eConnectedServerType)
	: m_eConnectedServerType(m_eConnectedServerType)
{}



void NetClientEventListener::OnConnected(Session* session) {
	ListenerClientBase::OnConnected(session);
	SyncConnectionResult(m_eConnectedServerType, session, true, 0);
}
void NetClientEventListener::OnDisconnected(Session* session) {
	ListenerClientBase::OnDisconnected(session);
	SyncDisconnectionResult(m_eConnectedServerType, session);
}

void NetClientEventListener::OnConnectFailed(Session* session, Int32U errorCode) {
	ListenerClientBase::OnConnectFailed(session, errorCode);
	SyncConnectionResult(m_eConnectedServerType, session, false, errorCode);
}

void NetClientEventListener::OnSent(Session* session, ISendPacket* sendPacket, Int32UL sentBytes) {
	ListenerClientBase::OnSent(session, sendPacket, sentBytes);
}

void NetClientEventListener::OnReceived(Session* session, ICommand* recvCmd) {
	ListenerClientBase::OnReceived(session, recvCmd);
	SyncReceivedCommand(m_eConnectedServerType, session, recvCmd);
}

void NetClientEventListener::OnReceived(Session* session, IRecvPacket* recvPacket) {
	ListenerClientBase::OnReceived(session, recvPacket);
}

void NetClientEventListener::SyncConnectionResult(ClientConnectServerType_t listenerType, Session* session, bool success, Int32U errorCode) {
	ConnectionSynchronizer* pSynchronizer = CoreNet_v->getConnectionSynchronizer();

	if (pSynchronizer == nullptr) {
		SGString szResult = StringUtil::ToString(success);
		_LogWarn_("동기화기가 소멸되어서 Connection의 후속처리를 수행할 수 없습니다. [타입:%d][결과:%s]", listenerType, szResult.Source());
		return;
	}

	pSynchronizer->enqueueConnection(listenerType, session, success, errorCode);
}

void NetClientEventListener::SyncDisconnectionResult(ClientConnectServerType_t listenerType, Session* session) {
	ConnectionSynchronizer* pSynchronizer = CoreNet_v->getConnectionSynchronizer();

	if (pSynchronizer == nullptr) {
		_LogWarn_("동기화기가 소멸되어서 Disconnection의 후속처리를 수행할 수 없습니다. [타입:%d]", listenerType);
		return;
	}

	pSynchronizer->enqueueDisconnection(listenerType, session);
}

void NetClientEventListener::SyncReceivedCommand(ClientConnectServerType_t listenerType, SGSession* session, ICommand* cmd) {
	CommandSynchronizer* pSynchronizer = CoreNet_v->getCommandSynchronizer();

	if (pSynchronizer == nullptr) {
		_LogWarn_("커맨드를 수신했지만 동기화기가 소멸되어서 이를 메인쓰레드에서 받아서 처리할 수가 없습니다. (커맨드:%d)", cmd->GetCommand());
		return;
	}

	pSynchronizer->enqueueCommand(listenerType, session, cmd);
}

