#include "Server.h"
#include "MysqlDatabase.h"

#include <SteinsGate/Common/MysqlConnectionPool.h>
#include <SteinsGate/Common/MysqlStatementBuilder.h>

USING_NS_JC;
USING_NS_JC;
USING_NS_STD;

MysqlDatabase::~MysqlDatabase() {
	DeleteSafe(m_pIocp);
}

MysqlDatabase* MysqlDatabase::GetInstance() {
	if (ms_pInstance == nullptr) {
		ms_pInstance = new MysqlDatabase();
	}

	return ms_pInstance;
}

bool MysqlDatabase::Initialize() {

	if (m_pConnectionPool == nullptr)
		m_pConnectionPool = new MysqlConnectionPool(DB_HOST, DB_PORT, DB_ID, DB_PASS, DB_NAME, 50);

	// 커넥션 25개 풀 초기화
	// 프로그램 종료 될때 알아서 연결들 모두 종료함
	if (!m_pConnectionPool->Init(DB_CONN_POOL_SIZE)) {
		_LogError_("DB 커넥션 풀 초기화 실패");
		return false;
	}

	// 빌더 커넥션 초기화
	// ㄹㅇ; String Escape 하나를 위해서 어쩔수없이 초기화함;
	if (!MysqlStatementBuilder::Initialize()) {
		_LogError_("DB 스테이트먼트 빌더 초기화 실패");
		return false;
	}

	_LogInfo_("데이터베이스 커넥션 풀(%d)이 성공적으로 초기화되었습니다. [localhost/%d]", DB_CONN_POOL_SIZE, DB_PORT);

	if (!m_pIocp->Create(4)) {
		return false;
	}


	_LogInfo_("데이터베이스 IOCP가 성공적으로 생서되었습니다.");

	m_pIocp->Run();
	return true;
}

bool MysqlDatabase::Finalize() {
	while (m_pIocp->GetPendingCount() != 0) {}

	m_pIocp->Join();

	if (!m_pIocp->Destroy()) {
		return false;
	}

	if (m_pConnectionPool)
		DeleteSafe(m_pConnectionPool);


	MysqlStatementBuilder::Finalize();

	return true;
}
