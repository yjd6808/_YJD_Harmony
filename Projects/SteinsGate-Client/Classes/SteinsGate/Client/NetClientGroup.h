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

	SGTcpClient* AuthTcp;
	SGTcpClient* LobbyTcp;
	SGTcpClient* GameTcp;
	SGUdpClient* GameUdp;
	SGTcpClient* AreaTcp;
	SGTcpClient* ChatTcp;
private:
	SGVector<NetClientEventListener*> m_vListeners;
	SGCommandParser m_Parser;

	friend class NetCore;
};


