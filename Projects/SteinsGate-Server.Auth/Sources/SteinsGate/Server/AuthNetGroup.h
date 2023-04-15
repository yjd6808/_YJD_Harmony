/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:26:14 PM
 * =====================
 *
 */


#pragma once

#include <Jnetwork/NetGroup.h>

#include <SteinsGate/Server/AuthServer.h>
#include <SteinsGate/Server/ListenerAuthServer.h>
#include <SteinsGate/Server/ListenerCenterClient.h>
#include <SteinsGate/Server/ListenerInterServerClient.h>


class ListenerAuthServer;
class AuthNetGroup final : public SGNetGroup
{
public:
	AuthNetGroup();
	~AuthNetGroup() override;
	void Initialize() override;
	bool ConnectCenterServer(int tryCount);

	SGTcpServer* Server;						// 인증 서버 역할
	SGTcpClient* CenterClient;					// 중앙 서버와 통신용
	SGUdpClient* InterServerClient;				// 각종 서버와 통신용
private:
	ListenerAuthServer m_AuthServerEventListener;
	ListenerCenterClient m_AuthCenterClientEventLIstener;
	ListenerInterServerClient m_InterServerEventListener;
};


