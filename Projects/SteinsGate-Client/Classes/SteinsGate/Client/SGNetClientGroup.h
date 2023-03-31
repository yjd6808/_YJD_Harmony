/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:26:14 PM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Client/SGNetAuthEventListener.h>
#include <SteinsGate/Client/SGNetLobbyEventListener.h>

class SGNetClientGroup final : public SGNetGroup
{
public:
	SGNetClientGroup();
	~SGNetClientGroup() override;

	void Initialize() override;
private:
	void InitAuthCmd();
	void InitLobbyCmd();
	void InitGameCmd();
	void InitAreaCmd();
	void InitChatCmd();

	SGTcpClientPtr m_spAuthTcp;
	SGTcpClientPtr m_spLobbyTcp;
	SGTcpClientPtr m_spGameTcp;
	SGUdpClientPtr m_spGameUdp;
	SGTcpClientPtr m_spAreaTcp;
	SGTcpClientPtr m_spChatTcp;
	
	SGClientNetEventListener m_AuthEventListener;
	SGClientNetEventListener m_LobbyEventListener;
	SGClientNetEventListener m_GameEventListener;
	SGClientNetEventListener m_ChatEventListener;
	SGClientNetEventListener m_AreaEventListener;

	SGCommandParser m_AuthParser;
	SGCommandParser m_LobbyParser;
	SGCommandParser m_LobbyParser;
};


