#include "Server.h"
#include "MysqlDatabase.h"

#include <SteinsGate/Common/MysqlConnectionPool.h>
#include <SteinsGate/Common/MysqlStatementBuilder.h>

USING_NS_JC;
USING_NS_JC;
USING_NS_JNET;
USING_NS_STD;

MysqlDatabase::MysqlDatabase(DatabaseInfo* info)
	: m_pIocp(nullptr)
	, m_pConnectionPool(nullptr)
	, m_pInfo(info)
	, m_bInitialized(false)
{}

MysqlDatabase::~MysqlDatabase() {
	if (m_bInitialized) {
		Finalize();
		JCORE_DELETE_SAFE(m_pIocp);
	}
}


bool MysqlDatabase::Initialize(ServerProcessType_t serverProcessType) {

	if (m_bInitialized) {
		_LogError_("이미 생성된 객체입니다.");
		return false;
	}

	const int iUse = m_pInfo->Use[serverProcessType];
	const int iConnectionPoolSize = m_pInfo->ConnectionPoolSize[serverProcessType];
	const int iMaxConnection = m_pInfo->MaxConnection[serverProcessType];
	const int iThreadCount = m_pInfo->IocpThreadCount[serverProcessType];

	if (iUse != 1) {
		_LogError_("해당 데이터베이스는 사용하지 않습니다.");
		return false;
	}



	if (m_pConnectionPool == nullptr)
		m_pConnectionPool = dbg_new MysqlConnectionPool(
			m_pInfo->HostName, 
			m_pInfo->ConnectionPort, 
			m_pInfo->AccountId, 
			m_pInfo->AccountPass, 
			m_pInfo->SchemaName,
			iMaxConnection
		);

	// 커넥션 풀 초기화
	if (!m_pConnectionPool->Init(iConnectionPoolSize)) {
		JCORE_DELETE_SAFE(m_pConnectionPool);
		_LogError_("DB 커넥션 풀 초기화 실패");
		return false;
	}

	// 빌더 커넥션 초기화
	// String Escape 하나를 위해서 어쩔수없이 초기화함;
	if (!MysqlStatementBuilder::Initialize(m_pInfo)) {
		_LogError_("DB 스테이트먼트 빌더 초기화 실패");
		return false;
	}

	_LogInfo_("데이터베이스 커넥션 풀(크기: %d) 초기화 [%s:%d]", 
		iConnectionPoolSize,
		m_pInfo->HostName.Source(), 
		m_pInfo->ConnectionPort
	);

	m_pIocp = dbg_new IOCP(iThreadCount);
	m_pIocp->Run();
	m_bInitialized = true;
	_LogInfo_("%s IOCP 실행완료 (쓰레드 수: %d)", m_pInfo->Name.Source(), iThreadCount);
	return true;
}

void MysqlDatabase::Finalize() {

	if (m_bInitialized == false)
		return;

	m_bInitialized = false;

	while (m_pIocp->GetPendingCount() != 0) {}

	m_pIocp->Join();
	m_pIocp->Destroy();

	if (m_pConnectionPool)
		JCORE_DELETE_SAFE(m_pConnectionPool);


	MysqlStatementBuilder::Finalize();
}


