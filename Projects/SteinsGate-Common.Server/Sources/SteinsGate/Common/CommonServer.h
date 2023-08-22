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
#include <SteinsGate/Common/IUpdatable.h>

class CommonServer
	: public JNetwork::TcpServer
	, public IUpdatable
{
protected:
	CommonServer(const JNetwork::IOCPPtr& iocp, const JCore::MemoryPoolAbstractPtr& bufferAllocator);
public:
	void SetBootState(ServerBootState_t state) { m_eBootState = state; }
	ServerBootState_t GetBootState() { return (ServerBootState_t)m_eBootState.Load(); }

	virtual ServerType_t GetServerType() = 0;
	const char* GetServerName() { return ServerType::Name[GetServerType()]; }

	void OnStarted() override;
	void OnStartFailed(Int32U errorCode) override;
	void OnStopped() override;

	void OnUpdate(const JCore::TimeSpan& elapsed) override;
private:
	JCore::AtomicInt m_eBootState;
};


