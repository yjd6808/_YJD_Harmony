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

struct NetServerInfo;

NS_SG_BEGIN

class NetSession;
class NetServer : public jnet::TcpServer, public IUpdatable
{
public:
	NetServer(const jnet::IOCPPtr& _pIocp, const jc::MemoryPoolAbstractPtr& _pBufferAllocator);
	virtual jnet::TcpSession* CreateSession() override;

	void SetServerId(int _serverId) { serverId_ = _serverId; }
	void SetBootState(ServerBootState_t _state) { bootState_ = _state; }
	void SetServerInfo(const NetServerInfo& _info) { serverInfo_ = _info; }

	ServerBootState_t GetBootState() { return (ServerBootState_t)bootState_.Load(); }
	const NetServerInfo& GetServerInfo() const { return serverInfo_; }
	int GetServerId() const { return serverId_; }

	bool Start();
	virtual const char* GetName() const override { return serverInfo_.serverName_.SafeSource(); }

	virtual void OnStarted() override;
	virtual void OnStartFailed(_u32 _errorCode) override;
	virtual void OnStopped() override;
	virtual void OnUpdate(const jc::TimeSpan& _elapsed) override;

private:
	int serverId_ = -1;
	jc::AtomicInt bootState_;
	NetServerInfo& serverInfo_;
};

NS_END