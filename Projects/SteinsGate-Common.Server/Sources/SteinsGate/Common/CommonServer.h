/*
 * 작성자: 윤정도
 * 생성일: 4/21/2023 9:02:03 PM
 * =====================
 *
 */


#pragma once

#include <JCore/Threading/Pulser.h>
#include <JNetwork/Host/TcpServer.h>
#include <SteinsGate/Common/ServerEnum.h>


class CommonServer : public JNetwork::TcpServer
{
protected:
	CommonServer(const JNetwork::IOCPPtr& iocp, const JCore::MemoryPoolAbstractPtr& bufferAllocator);
public:
	bool ProcessOrder(CenterOrder_t order);
	void ProcessLoop(JCore::PulserStatistics* pulseStat);

	void SetBootState(ServerBootState_t state) { m_eBootState = state; }
	ServerBootState_t GetBootState() { return (ServerBootState_t)m_eBootState.Load(); }

	virtual void OnLoop(JCore::PulserStatistics* pulseStat) = 0;
	virtual ServerType_t GetServerType() = 0;
	const char* GetServerName() { return ServerType::Name[GetServerType()]; }
	virtual ServerInfo GetServerInfo() = 0;
private:
	JCore::AtomicInt m_eBootState;
};


