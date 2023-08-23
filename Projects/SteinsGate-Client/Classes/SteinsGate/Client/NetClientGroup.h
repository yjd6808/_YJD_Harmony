/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:26:14 PM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Client/NetClientEventListener.h>

class NetClientGroup final : public SGNetGroup
{
public:
	NetClientGroup();
	~NetClientGroup() override;

	void Initialize() override;
	void Finalize() override;

	SGTcpClient* AuthTcp;
	SGUdpClient* AuthUdp;
	SGTcpClient* LobbyTcp;
	SGUdpClient* LobbyUdp;
	SGTcpClient* LogicTcp;
	SGUdpClient* LogicUdp;
	SGTcpClient* AreaTcp;
	SGUdpClient* AreaUdp;
	SGTcpClient* ChatTcp;
	SGUdpClient* ChatUdp;

	SGCommandParser* getParser(ClientConnectServerType_t serverType);
private:
	SGCommandParser m_Parser[ClientConnectServerType::Max];

	friend class NetCore;
};


