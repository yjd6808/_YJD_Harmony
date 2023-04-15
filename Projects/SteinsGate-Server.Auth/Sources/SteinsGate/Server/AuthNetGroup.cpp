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


	struct Region
	{
		Region(int k, int g) : a(k), b(g) {}
		int a;
		int b;
	};
	auto task = IOCPTask<int>::Run(m_spIOCP.GetPtr(), [](IOCPTaskResult<int>& result) {

		for (int i = 0; i < 5; ++i) {
			_NetLogDebug_("%d", i);
			Thread::Sleep(1000);
		}
		result.Value = 20;
	});

	task = IOCPTask<Region>::Run(m_spIOCP.GetPtr(), [](IOCPTaskResult<Region>& result) {

		for (int i = 0; i < 5; ++i) {
			_NetLogDebug_("%d", i);
			Thread::Sleep(1000);
		}

		result.Value.a = 30;
		result.Value.b = 40;
	}, nullptr, 1, 2);


	task = IOCPTask<void>::Run(m_spIOCP.GetPtr(), [](IOCPTaskResult<void>& result) {

		for (int i = 0; i < 5; ++i) {
			_NetLogDebug_("%d", i);
			Thread::Sleep(1000);
		}

	}, [](IOCPTaskResult<void>& result) {
		Console::WriteLine("오하요");
	});

	char s[] = "wjdeh616";
	auto task2 = CoreGameDB_v->QueryAsync("insert into t_account (c_id, c_pass) values (?, ?)", s, s);

	auto& r = task2->Wait();
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

