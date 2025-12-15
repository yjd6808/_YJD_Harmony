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
	NetClientEventListener(ClientConnectServerType_t _connectedServerType);

protected:
	void OnConnected(JNetwork::Session* _pSession) override;
	void OnDisconnected(JNetwork::Session* _pSession, Int32U _errorCode) override;
	void OnConnectFailed(JNetwork::Session* _pSession, Int32U _errorCode) override;
	void OnSent(JNetwork::Session* _pSession, JNetwork::IPacket* _pSendPacket, Int32UL _sentBytes) override;
	void OnReceived(JNetwork::Session* _pSession, JNetwork::ICommand* _pRecvCmd) override;
	void OnReceived(JNetwork::Session* _pSession, JNetwork::RecvedCommandPacket* _pRecvPacket) override;

	// 동기화 수행
	static void SyncConnectionResult(ClientConnectServerType_t _listenerType, JNetwork::Session* _pSession,
	                                 bool _success, Int32U _errorCode);
	static void SyncDisconnectionResult(ClientConnectServerType_t _listenerType, JNetwork::Session* _pSession);
	static void SyncReceivedCommand(ClientConnectServerType_t _listenerType, SGSession* _pSession,
	                                JNetwork::ICommand* _pCmd);

private:
	ClientConnectServerType_t connectedServerType_;
};
