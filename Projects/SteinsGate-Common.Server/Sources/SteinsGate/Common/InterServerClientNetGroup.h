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
	~InterServerClientNetGroup() override;

	void Initialize() override;
	void Finalize() override;

	bool ConnectCenterServer(int tryCount);
	void ProcessUpdate(const JCore::TimeSpan& elpased);

	JNetwork::TcpClient* GetInterServerClientTcp() const { return m_pInterServerClientTcp; }
	JNetwork::UdpClient* GetInterServerClientUdp() const { return m_pInterServerClientUdp; }
protected:
	virtual void InitializeIOCP() = 0;
	virtual void InitializeBufferPool() = 0;
	virtual void InitializeParser();
	virtual void InitializeInterServerTcp() = 0;
	virtual void InitializeInterServerUdp() = 0;
	

	virtual void OnUpdate(const JCore::TimeSpan& elapsed) = 0;
	virtual bool IsPeerServer() { return true; }	// 중앙 서버만 false

	void SyncPeerServerTime(const JCore::TimeSpan& elapsed);

	JNetwork::TcpClient* m_pInterServerClientTcp;
	JNetwork::UdpClient* m_pInterServerClientUdp;
	JNetwork::CommandParser* m_pParser;
};
