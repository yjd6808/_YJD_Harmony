/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 3:53:19 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/CommonServer.h>
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

	SGTcpSession* CreateSession() override;
	ServerInfo GetServerInfo() override;
	ServerType_t GetServerType() override { return ServerType::Center; }

	CenterSession* GetCenterSession(CenterClientType_t clientType) { return m_pSession[clientType]; }
	void SetCenterSession(CenterClientType_t clientType, CenterSession* session) { m_pSession[clientType] = session; }
	bool IsAllCenterSessionConnected();
	bool IsConnected(CenterSession* session);

	void SetStartupLaunching(bool startupLaunching) { m_bStartupLaunching = startupLaunching; }
	bool IsStartupLaunching() { return m_bStartupLaunching; }
	void OnLoop(int sleepMs) override;
private:
	JCore::Atomic<CenterSession*> m_pSession[CenterClientType::Max];
	bool m_bStartupLaunching;	// 모든 서버세션들이 접속완료되어서 서버 시작명령을 내렸는지 여부
};
