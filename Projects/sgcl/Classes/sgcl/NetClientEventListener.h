/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:36:49 PM
 * =====================
 *
 */


#pragma once

#include <sgcl/Core.h>

#include <sg/ListenerClientBase.h>
#include <sg/ListenerHelperBase.h>


class NetClientEventListener : public ListenerClientBase
{
public:
	NetClientEventListener(ClientConnectServerType_t _connectedServerType);

protected:
	void OnConnected(jnet::Session* _pSession) override;
	void OnDisconnected(jnet::Session* _pSession, Int32U _errorCode) override;
	void OnConnectFailed(jnet::Session* _pSession, Int32U _errorCode) override;
	void OnSent(jnet::Session* _pSession, jnet::IPacket* _pSendPacket, Int32UL _sentBytes) override;
	void OnReceived(jnet::Session* _pSession, jnet::ICommand* _pRecvCmd) override;
	void OnReceived(jnet::Session* _pSession, jnet::RecvedCommandPacket* _pRecvPacket) override;

	// 동기화 수행
	static void SyncConnectionResult(ClientConnectServerType_t _listenerType, jnet::Session* _pSession, bool _success, Int32U _errorCode);
	static void SyncDisconnectionResult(ClientConnectServerType_t _listenerType, jnet::Session* _pSession);
	static void SyncReceivedCommand(ClientConnectServerType_t _listenerType, jnet::Session* _pSession, jnet::ICommand* _pCmd);

private:
	ClientConnectServerType_t connectedServerType_;
};
