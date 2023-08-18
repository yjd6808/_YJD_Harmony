/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:36:49 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/Tutturu.h>

#include <SteinsGate/Common/ListenerClientBase.h>
#include <SteinsGate/Common/ListenerHelperBase.h>



class NetClientEventListener : public ListenerClientBase
{
public:
	NetClientEventListener(ClientConnectServerType_t m_eConnectedServerType);
protected:
	void OnConnected(JNetwork::Session* session) override;
	void OnDisconnected(JNetwork::Session* session) override;
	void OnConnectFailed(JNetwork::Session* session, Int32U errorCode) override;
	void OnSent(JNetwork::Session* session, JNetwork::ISendPacket* sendPacket, Int32UL sentBytes) override;
	void OnReceived(JNetwork::Session* session, JNetwork::ICommand* recvCmd) override;
	void OnReceived(JNetwork::Session* session, JNetwork::IRecvPacket* recvPacket) override;

	// 동기화 수행
	static void SyncConnectionResult(ClientConnectServerType_t listenerType, JNetwork::Session* session, bool success, Int32U errorCode);
	static void SyncDisconnectionResult(ClientConnectServerType_t listenerType, JNetwork::Session* session);
	static void SyncReceivedCommand(ClientConnectServerType_t listenerType, SGSession* session, JNetwork::ICommand* cmd);
private:
	ClientConnectServerType_t m_eConnectedServerType;
};


