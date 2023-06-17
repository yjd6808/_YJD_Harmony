/*
 * 작성자: 윤정도
 * 생성일: 6/16/2023 2:24:28 AM
 * =====================
 * 모든 서버 프로세스의 공통 인터서버 클라이언트 그룹
 * 단, 중앙서버는 릴레이 주체이므로 TcpClient는 사용하지 않는다.
 */


#pragma once

#include <JNetwork/NetGroup.h>

class JCORE_NOVTABLE InterServerClientNetGroup : public JNetwork::NetGroup
{
public:
	InterServerClientNetGroup();

	void Initialize() override;
	bool ConnectCenterServer(int tryCount);
	void ProcessLoop(JCore::PulserStatistics* pulseStat);

	JNetwork::TcpClient* GetInterServerClientTcp() const { return m_pInterServerClientTcp; }
	JNetwork::UdpClient* GetInterServerClientUdp() const { return m_pInterServerClientUdp; }
protected:
	virtual void InitializeIOCP() = 0;
	virtual void InitializeBufferPool() = 0;
	virtual void InitializeInterServerTcp() = 0;
	virtual void InitializeInterServerUdp() = 0;

	virtual void OnLoop(JCore::PulserStatistics* pulseStat) = 0;

	JNetwork::TcpClient* m_pInterServerClientTcp;
	JNetwork::UdpClient* m_pInterServerClientUdp;
};
