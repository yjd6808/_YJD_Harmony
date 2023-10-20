#include "Pch.h"
#include "MysqlDatabase.h"

#include <TF/Server/Database/MysqlConnectionPool.h>
#include <TF/Server/Database/MysqlStatementBuilder.h>

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

	JCORE_DELETE_SAFE(m_pInfo);
}


bool MysqlDatabase::Initialize() {

	if (m_bInitialized) {
		_LogError_("이미 생성된 객체입니다.");
		return false;
	}

	const int iUse = m_pInfo->Use;
	const int iConnectionPoolSize = m_pInfo->ConnectionPoolSize;
	const int iMaxConnection = m_pInfo->MaxConnection;
	const int iThreadCount = m_pInfo->IocpThreadCount;

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
	m_pIocp->SetName(m_pInfo->Name);
	m_pIocp->Run();
	m_bInitialized = true;
	_LogInfo_("%s %s 실행완료 (쓰레드 수: %d)", m_pInfo->Name.Source(), IOCP::TypeName(), iThreadCount);
	return true;
}

void MysqlDatabase::Finalize() {

	if (m_bInitialized == false)
		return;

	_LogInfo_("%s 파괴시작", m_pInfo->Name.Source());
	m_bInitialized = false;

	m_pIocp->Join();
	_LogInfo_("%s %s 쪼인완료", m_pInfo->Name.Source(), IOCP::TypeName());

	m_pIocp->Destroy();
	_LogInfo_("%s %s 파괴완료", m_pInfo->Name.Source(), IOCP::TypeName());

	if (m_pConnectionPool)
		JCORE_DELETE_SAFE(m_pConnectionPool);


	MysqlStatementBuilder::Finalize();
}

const String& MysqlDatabase::GetName() const {
	if (m_pInfo == nullptr) {
		return String(0);
	}

	return m_pInfo->Name;
}

