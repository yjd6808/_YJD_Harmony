/*
 * 작성자: 윤정도
 * 생성일: 4/21/2023 1:42:49 PM
 * =====================
 *
 */


#pragma once

#include <JNetwork/NetGroup.h>

#include <Jnetwork/Host/TcpServer.h>
#include <Jnetwork/Host/TcpClient.h>
#include <Jnetwork/Host/UdpClient.h>

class JCORE_NOVTABLE CommonNetGroup : public JNetwork::NetGroup
{
public:
	CommonNetGroup();

	void Initialize() override;
	void ProcessLoop(JCore::PulserStatistics* pulseStat);

	JNetwork::TcpServer* GetServer() { return m_pServer; }
protected:
	
	virtual void InitializeIOCP() = 0;
	virtual void InitializeBufferPool() = 0;
	virtual void InitializeServer() = 0;

	virtual void OnLoop(JCore::PulserStatistics* pulseStat) = 0;

	JNetwork::TcpServer* m_pServer;
};