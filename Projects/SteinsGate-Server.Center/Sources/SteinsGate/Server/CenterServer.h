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
	CenterServer(
		const JNetwork::IOCPPtr& iocp,
		const JCore::MemoryPoolAbstractPtr& bufferAllocator,
		JNetwork::ServerEventListener* eventListener,
		int maxConn,
		int sessionRecvBufferSize,
		int sessionSendBufferSize
	);
	~CenterServer() override;

	SGTcpSession* CreateSession() override;
	ServerInfo GetServerInfo() override;
	ServerType_t GetServerType() override { return ServerType::Center; }

	CenterSession* GetCenterSession(int serverId) { return m_pSession[serverId].Session; }
	void AddSession(CenterSession* session);
	void RemoveSession(CenterSession* session);

	bool IsAllCenterSessionConnected();
	bool IsConnected(CenterSession* session);
	bool IsConnected(int serverId);

	void SetStartupLaunching(bool startupLaunching) { m_bStartupLaunching = startupLaunching; }
	bool IsStartupLaunching() { return m_bStartupLaunching; }
	void OnLoop(JCore::PulserStatistics* pulserStat) override;
private:
	ANONYMOUS_CACHE_ALIGNED_VAR(
		CenterSession* Session = nullptr;
		InterServerClientType_t Type = InterServerClientType::None;
	) m_pSession[MaxServerId_v];
	bool m_bStartupLaunching;	// 모든 서버세션들이 접속완료되어서 서버 시작명령을 내렸는지 여부
};
