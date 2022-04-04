#include <mysql.h>

#define _WINSOCK2API_		// winsock2 �ߺ� ��Ŭ��� ����

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

	// Ŀ�ؼ� 25�� Ǯ �ʱ�ȭ
	// ���α׷� ���� �ɶ� �˾Ƽ� ����� ��� ������
	if (!TFDBConnPool.GetInstance().Init(DB_CONN_POOL_SIZE)) {
		Console::WriteLine(ConsoleColor::RED, "DB Ŀ�ؼ� Ǯ �ʱ�ȭ ����");
		return false;
	}

	// ���� Ŀ�ؼ� �ʱ�ȭ
	// ����; String Escape �ϳ��� ���ؼ� ��¿������ �ʱ�ȭ��;
	if (!MysqlStatementBuilder::Initialize()) {
		Console::WriteLine(ConsoleColor::RED, "DB ������Ʈ��Ʈ ���� �ʱ�ȭ ����");
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
	m_pIocp->Join();

	if (!m_pIocp->Destroy()) {
		return false;
	}


	return true;
}
