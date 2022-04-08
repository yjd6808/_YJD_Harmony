#include <TF/PrecompiledHeader.h>
#include <TF/Database/MysqlDatabase.h>
#include <TF/Database/MysqlConnectionPool.h>
#include <TF/ServerConfiguration.h>
#include <TF/Util/Console.h>
#include <TF/ServerConfiguration.h>
#include <TF/Database/MysqlStatementBuilder.h>

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

	// Ŀ�ؼ� 25�� Ǯ �ʱ�ȭ
	// ���α׷� ���� �ɶ� �˾Ƽ� ����� ��� ������
	if (!m_pConnectionPool->Init(DB_CONN_POOL_SIZE)) {
		Console::WriteLine(ConsoleColor::LIGHTGRAY, "DB Ŀ�ؼ� Ǯ �ʱ�ȭ ����");
		return false;
	}

	// ���� Ŀ�ؼ� �ʱ�ȭ
	// ����; String Escape �ϳ��� ���ؼ� ��¿������ �ʱ�ȭ��;
	if (!MysqlStatementBuilder::Initialize()) {
		Console::WriteLine(ConsoleColor::LIGHTGRAY, "DB ������Ʈ��Ʈ ���� �ʱ�ȭ ����");
		return false;
	}

	Console::WriteLine(ConsoleColor::GREEN, "�����ͺ��̽� Ŀ�ؼ� Ǯ(%d)�� ���������� �ʱ�ȭ�Ǿ����ϴ�. [localhost/%d]", DB_CONN_POOL_SIZE, DB_PORT);

	if (!m_pIocp->Create(4)) {
		return false;
	}


	Console::WriteLine(ConsoleColor::GREEN, "�����ͺ��̽� IOCP�� ���������� �����Ǿ����ϴ�.");

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
