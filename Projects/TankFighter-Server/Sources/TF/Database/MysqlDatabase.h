/*
 * �ۼ��� : ������
 * DB
 */

#pragma once

#include <TF/Overlapped/IOCPOverlappedQuery.h>
#include <TF/Database/MysqlConnectionPool.h>
#include <TF/Database/MysqlStatementBuilder.h>
#include <TF/Database/MysqlQuery.h>

#define _MysqlConnPool MysqlDatabase::GetInstance()->GetConnectionPool()

class MysqlQueryFuture;
class MysqlDatabase
{
private:
	MysqlDatabase()
		: m_pIocp(new JNetwork::IOCP)
		, m_pConnectionPool(nullptr) {
	}
	
public:
	virtual ~MysqlDatabase();
	static MysqlDatabase* GetInstance();
	
	bool Initialize();
	bool Finalize();

	MysqlConnectionPool* GetConnectionPool() const { return m_pConnectionPool; }

	/*
	 * �񵿱� Query ���� �� Wait() �Լ��� ȣ���ؼ� ������� ���� �� �޾ƺ� �� �ִ�.
     *	
	 * ����� �޾Ƽ� ����� �� �׸��� ���������� ����ϱ� ���� 2�� ��������
	 * ǻ�Ĵ� �� �Լ� �ۿ��� �� Release�� ���� ��
	*/
	template <typename... Args>
	MysqlQueryFuture* QueryAsync(const std::string& statement, Args&&... args) {
		const JCore::String preparedStatement = MysqlStatementBuilder::Build(statement, JCore::Forward<Args>(args)...);

		if (preparedStatement == "")
			return false;

		MysqlQueryFuture* future = new MysqlQueryFuture(preparedStatement);
		future->AddRef(2);	
		IOCPOverlappedQuery* pOverlapped = new IOCPOverlappedQuery(m_pIocp, future);

		if (m_pIocp->Post(0, NULL, pOverlapped) == FALSE) {
			DebugAssert(false, "MysqlDatabase::QueryAsync() Failed");
			pOverlapped->Release();
			future->Release(2);
			return nullptr;
		}

		
		return future;
	}

	/*
	 * ����ȭ Query ����
	 * ����Ʈ �����ͷ� �˾Ƽ� �������ֹǷ� �������.
	 */

	template <typename... Args>
	JCore::SharedPointer<MysqlQuery> Query(const std::string& statement, Args&&... args) {
		if (statement.length() <= 6) {
			DebugAssert(false, "MysqlDatabase::Query() ������ �ȹٷ� �Է����ּ���.");
			return nullptr;
		}

		auto pConn = _MysqlConnPool->GetConnection();

		if (pConn == nullptr) {
			// ����
			DebugAssert(false, "MysqlDatabase::Query() Ŀ�ؼ� Ǯ���� �������� ����");
			return nullptr;
		}

		AutoReleaseConnection autoRelease(pConn, _MysqlConnPool);
		const JCore::String preparedStatement = MysqlStatementBuilder::Build(statement, JCore::Forward<Args>(args)...);

		if (preparedStatement == "")
			return nullptr;

		
		JCore::SharedPointer<MysqlQuery> spQuery = JCore::MakeShared<MysqlQuery>(pConn, preparedStatement);
		spQuery->ExecuteAuto();

		return spQuery;
	}
private:
	JNetwork::IOCP* m_pIocp;
	MysqlConnectionPool* m_pConnectionPool;

	// ���� ���� ���
	// ���� �� ó���� �͵��� ����ٰ� �߰� �ϸ� �ȴ�

	inline static MysqlDatabase* ms_pInstance = nullptr;

};