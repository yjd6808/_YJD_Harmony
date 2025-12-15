/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 3:53:19 PM
 * =====================
 *
 */


#pragma once

#include <JCore/Aligner.h>

#include <SteinsGate/Common/CommonServer.h>
#include <SteinsGate/Common/Const.h>
#include <SteinsGate/Common/Type.h>
#include <SteinsGate/Server/CenterSession.h>

class CenterServer final : public CommonServer
{
public:
	CenterServer(const JNetwork::IOCPPtr& _pIocp, const JCore::MemoryPoolAbstractPtr& _pBufferAllocator);
	~CenterServer() override;

	SGTcpSession* CreateSession() override;
	ServerType_t GetServerType() override { return ServerType::Center; }

	CenterSession* GetCenterSession(int _serverId) { return centerSessions_[_serverId].Session; }
	void AddSession(CenterSession* _pSession);
	void RemoveSession(CenterSession* _pSession);
	void BroadcastPacket(JNetwork::IPacket* _pPacket);

	bool IsAllCenterSessionConnected();
	bool IsConnected(CenterSession* _pSession);
	bool IsConnected(int _serverId);

	void SetStartupLaunching(bool _startupLaunching) { startupLaunching_ = _startupLaunching; }
	bool IsStartupLaunching() { return startupLaunching_; }
	void OnUpdate(const JCore::TimeSpan& _elapsed) override;

private:
	ANONYMOUS_CACHE_ALIGNED_VAR(
		CenterSession* Session = nullptr;
		ServerProcessType_t Type = ServerProcessType::None;
	) centerSessions_[Const::Server::MaxProcessId];
	bool startupLaunching_;    // 모든 서버세션들이 접속완료되어서 서버 시작명령을 내렸는지 여부
};
