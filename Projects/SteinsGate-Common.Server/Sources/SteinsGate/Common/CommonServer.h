/*
 * 작성자: 윤정도
 * 생성일: 4/21/2023 9:02:03 PM
 * =====================
 *
 */


#pragma once

#include <JNetwork/Host/TcpServer.h>
#include <SteinsGate/Common/ServerEnum.h>
#include <SteinsGate/Common/IUpdatable.h>

class CommonServer
	: public JNetwork::TcpServer
	  , public IUpdatable
{
protected:
	CommonServer(const JNetwork::IOCPPtr& _pIocp, const JCore::MemoryPoolAbstractPtr& _pBufferAllocator);

public:
	void SetBootState(ServerBootState_t _state) { bootState_ = _state; }
	ServerBootState_t GetBootState() { return (ServerBootState_t)bootState_.Load(); }

	virtual ServerType_t GetServerType() = 0;
	const char* GetServerName() { return ServerType::Name[GetServerType()]; }

	void OnStarted() override;
	void OnStartFailed(Int32U _errorCode) override;
	void OnStopped() override;

	void OnUpdate(const JCore::TimeSpan& _elapsed) override;

private:
	JCore::AtomicInt bootState_;
};
