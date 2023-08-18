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
	void Finalize() override;

	void ProcessUpdate(const JCore::TimeSpan& elapsed);

	JNetwork::TcpServer* GetServer() { return m_pServer; }
protected:
	
	virtual void InitializeIOCP() = 0;
	virtual void InitializeBufferPool() = 0;
	virtual void InitializeServer() = 0;
	virtual void InitializeParser();

	virtual void OnUpdate(const JCore::TimeSpan& elapsed) = 0;

	JNetwork::TcpServer* m_pServer;
	JNetwork::CommandParser* m_pParser;
};