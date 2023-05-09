/*
 * 작성자: 윤정도
 * 생성일: 4/21/2023 9:02:03 PM
 * =====================
 *
 */


#pragma once

#include <JNetwork/Host/TcpServer.h>
#include <SteinsGate/Common/ServerEnum.h>

class CommonServer : public JNetwork::TcpServer
{
protected:
	CommonServer(
		const JNetwork::IOCPPtr& iocp,
		const JCore::MemoryPoolAbstractPtr& bufferAllocator,
		JNetwork::ServerEventListener* eventListener,
		int maxConn,
		int sessionRecvBufferSize,
		int sessionSendBufferSize
	);

public:
	bool ProcessOrder(CenterOrder_t order);
	void ProcessLoop(int sleepMs);

	void SetBootState(ServerBootState_t state) { m_eBootState = state; }
	ServerBootState_t GetBootState() { return (ServerBootState_t)m_eBootState.Load(); }

	virtual void OnLoop(int sleepMs) = 0;
	virtual ServerType_t GetServerType() = 0;
	virtual ServerInfo GetServerInfo() = 0;
private:
	JCore::AtomicInt m_eBootState;
};


