/*
 * 작성자: 윤정도
 * 생성일: 4/21/2023 9:02:03 PM
 * =====================
 *
 */


#pragma once

#include <jnet/Host/TcpServer.h>
#include <sgs/ServerEnum.h>
#include <sgs/IUpdatable.h>

class CommonServer
	: public jnet::TcpServer
	, public IUpdatable
{
protected:
	CommonServer(const jnet::IOCPPtr& _pIocp, const jc::MemoryPoolAbstractPtr& _pBufferAllocator);

public:
	void SetBootState(ServerBootState_t _state) { bootState_ = _state; }
	ServerBootState_t GetBootState() { return (ServerBootState_t)bootState_.Load(); }

	virtual ServerType_t GetServerType() = 0;
	const char* GetServerName() { return ServerType::Name[GetServerType()]; }

	void OnStarted() override;
	void OnStartFailed(Int32U _errorCode) override;
	void OnStopped() override;

	void OnUpdate(const jc::TimeSpan& _elapsed) override;

private:
	jc::AtomicInt bootState_;
};
