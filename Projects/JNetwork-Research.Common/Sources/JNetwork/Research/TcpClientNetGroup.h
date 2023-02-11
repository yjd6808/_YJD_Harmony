/*
 * 작성자: 윤정도
 * 생성일: 2/11/2023 4:04:21 PM
 * =====================
 * TCP 클라이언트 테스트
 */


#pragma once

#include <JNetwork/Host/TcpClient.h>
#include <JNetwork/NetGroup.h>

#include <JNetwork/Research/ClientListener.h>

NS_JNET_BEGIN

class TcpClientNetGroup : public NetGroup
{
public:
	TcpClientNetGroup();
	~TcpClientNetGroup() override;

	void Initialize() override;
	TcpClient* GetLogin() { return m_spLoginClient.GetPtr(); }
	TcpClient* GetChannel () { return m_spChannelClient.GetPtr(); }
	TcpClient* GetGame() { return m_spGameClient.GetPtr(); }
private:
	TcpClientWPtr m_spLoginClient;
	TcpClientWPtr m_spChannelClient;
	TcpClientWPtr m_spGameClient;

	ClientListener m_LoginListener;
	ClientListener m_ChannelListener;
	ClientListener m_GameListener;
};

NS_JNET_END