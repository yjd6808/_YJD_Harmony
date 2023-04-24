/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:26:14 PM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Client/NetAuthEventListener.h>
#include <SteinsGate/Client/NetLobbyEventListener.h>

class SGNetClientGroup final : public SGNetGroup
{
public:
	SGNetClientGroup();
	~SGNetClientGroup() override;

	void Initialize() override;
private:
	SGTcpClientPtr m_spAuthTcp;
	SGTcpClientPtr m_spLobbyTcp;
	SGTcpClientPtr m_spGameTcp;
	SGUdpClientPtr m_spGameUdp;
	SGTcpClientPtr m_spAreaTcp;
	SGTcpClientPtr m_spChatTcp;
	
	//SGNetClientEventListener m_AuthEventListener;
	//SGNetClientEventListener m_LobbyEventListener;
	//SGNetClientEventListener m_GameEventListener;
	//SGNetClientEventListener m_ChatEventListener;
	//SGNetClientEventListener m_AreaEventListener;

	SGCommandParser m_AuthParser;
	SGCommandParser m_LobbyParser;
	SGCommandParser m_GameParser;
};


