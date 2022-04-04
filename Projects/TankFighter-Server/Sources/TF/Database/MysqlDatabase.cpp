#include <mysql.h>

#define _WINSOCK2API_		// winsock2 중복 인클루드 방지

#include <TF/Database/MysqlDatabase.h>
#include <TF/Database/MysqlConnectionPool.h>
#include <TF/ServerConfiguration.h>
#include <TF/Util/Console.h>
#include <TF/ServerConfiguration.h>
#include <TF/Database/MysqlStatementBuilder.h>

MysqlDatabase* MysqlDatabase::GetInstance() {
	if (ms_pInstance == nullptr) {
		ms_pInstance = new MysqlDatabase();
	}

	return ms_pInstance;
}

bool MysqlDatabase::Initialize() {

	// 커넥션 25개 풀 초기화
	// 프로그램 종료 될때 알아서 연결들 모두 종료함
	if (!TFDBConnPool.GetInstance().Init(DB_CONN_POOL_SIZE)) {
		Console::WriteLine(ConsoleColor::RED, "DB 커넥션 풀 초기화 실패");
		return false;
	}

	// 빌더 커넥션 초기화
	// ㄹㅇ; String Escape 하나를 위해서 어쩔수없이 초기화함;
	if (!MysqlStatementBuilder::Initialize()) {
		Console::WriteLine(ConsoleColor::RED, "DB 스테이트먼트 빌더 초기화 실패");
		return false;
	}

	Console::WriteLine(ConsoleColor::GREEN, "데이터베이스 커넥션 풀(%d)이 성공적으로 초기화되었습니다. [localhost/%d]", DB_CONN_POOL_SIZE, DB_PORT);

	if (!m_pIocp->Create(4)) {
		return false;
	}


	Console::WriteLine(ConsoleColor::GREEN, "데이터베이스 IOCP가 성공적으로 생서되었습니다.");

	m_pIocp->Run();
	return true;
}

bool MysqlDatabase::Finalize() {
	m_pIocp->Join();

	if (!m_pIocp->Destroy()) {
		return false;
	}


	return true;
}
