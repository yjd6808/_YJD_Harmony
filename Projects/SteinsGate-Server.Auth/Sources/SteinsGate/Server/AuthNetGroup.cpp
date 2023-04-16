/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:26:49 PM
 * =====================
 *
 */

#include "Auth.h"
#include "AuthCoreHeader.h"
#include "AuthNetGroup.h"

USING_NS_JC;
USING_NS_JNET;

AuthNetGroup::AuthNetGroup()
	: Server(nullptr)
	, CenterClient(nullptr)
	, InterServerClient(nullptr)
{}

AuthNetGroup::~AuthNetGroup() {}

void AuthNetGroup::Initialize() {

	CreateIocp(2);
	CreateBufferPool({});
	RunIocp();

	constexpr int RecvBufferSize = 2048;
	constexpr int SendBufferSize = 2048;
	

	// 중앙 서버에 접속될떄까지 시도
	auto spCenterClient = MakeShared<TcpClient>(m_spIOCP, m_spBufferPool, &m_AuthCenterClientEventLIstener, RecvBufferSize, SendBufferSize);
	spCenterClient->Bind(CoreServer_v->Auth.BindCenterTcp);
	
	// 인증 서버 시작
	auto spServer = MakeShared<AuthServer>(m_spIOCP, m_spBufferPool, &m_AuthServerEventListener, CoreServer_v->Auth.MaxSessionCount, RecvBufferSize, SendBufferSize);
	spServer->Start(CoreServer_v->Auth.BindAuthTcp);

	// UDP 바인드
	auto spInterServerClient = MakeShared<UdpClient>(m_spIOCP, m_spBufferPool, &m_InterServerEventListener, RecvBufferSize, SendBufferSize);
	spInterServerClient->Bind(CoreServer_v->Auth.BindInterServerUdp);
	
	AddHost(spServer);
	AddHost(spCenterClient);
	AddHost(spInterServerClient);

	Server = spServer.Get<TcpServer*>();
	CenterClient = spCenterClient.Get<TcpClient*>();
	InterServerClient = spInterServerClient.Get<UdpClient*>();
}

bool AuthNetGroup::ConnectCenterServer(int tryCount) {
	DebugAssertMsg(CenterClient != nullptr, "센터 클라이언트가 초기화되어있지 않습니다.");

	constexpr int CenterConnectionTimeout = 1000;

	int iTryCount = 0;

	_LogDebug_("중앙 서버에 접속을 시도합니다...");
	while (!CenterClient->Connect(CoreServer_v->Center.RemoteCenter, CenterConnectionTimeout)) {
		++iTryCount;

		if (iTryCount >= tryCount) {
			break;
		}
	}

	return CenterClient->GetState() == Host::eConnected;
}

