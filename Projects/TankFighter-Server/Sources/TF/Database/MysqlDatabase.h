/*
 * 작성자 : 윤정도
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
	 * 비동기 Query 실행 후 Wait() 함수를 호출해서 결과값을 원할 때 받아볼 수 있다.
     *	
	 * 결과로 받아서 사용할 때 그리고 오버랩에서 사용하기 위해 2로 설정해줌
	 * 퓨쳐는 이 함수 밖에서 꼭 Release를 해줄 것
	*/
	template <typename... Args>
	MysqlQueryFuture* QueryAsync(const JCore::String& statement, Args&&... args) {
		const JCore::String preparedStatement = MysqlStatementBuilder::Build(statement, JCore::Forward<Args>(args)...);

		if (preparedStatement == "")
			return nullptr;

		MysqlQueryFuture* future = new MysqlQueryFuture(preparedStatement);
		future->AddRef(2);	
		IOCPOverlappedQuery* pOverlapped = new IOCPOverlappedQuery(m_pIocp, future);

		if (m_pIocp->Post(0, NULL, pOverlapped) == FALSE) {
			DebugAssertMsg(false, "MysqlDatabase::QueryAsync() Failed");
			pOverlapped->Release();
			future->Release(2);
			return nullptr;
		}

		
		return future;
	}

	/*
	 * 동기화 Query 실행
	 * 스마트 포인터로 알아서 해제해주므로 상관없다.
	 */

	template <typename... Args>
	JCore::SharedPtr<MysqlQuery> Query(const JCore::String& statement, Args&&... args) {
		if (statement.Length() <= 6) {
			DebugAssertMsg(false, "MysqlDatabase::Query() 쿼리를 똑바로 입력해주세요.");
			return nullptr;
		}

		auto pConn = _MysqlConnPool->GetConnection();

		if (pConn == nullptr) {
			// 실패
			DebugAssertMsg(false, "MysqlDatabase::Query() 커넥션 풀에서 가져오기 실패");
			return nullptr;
		}

		AutoReleaseConnection autoRelease(pConn, _MysqlConnPool);
		const JCore::String preparedStatement = MysqlStatementBuilder::Build(statement, JCore::Forward<Args>(args)...);

		if (preparedStatement == "")
			return nullptr;

		
		JCore::SharedPtr<MysqlQuery> spQuery = JCore::MakeShared<MysqlQuery>(pConn, preparedStatement);
		spQuery->ExecuteAuto();

		return spQuery;
	}
private:
	JNetwork::IOCP* m_pIocp;
	MysqlConnectionPool* m_pConnectionPool;

	// 쿼리 수행 통계
	// 실패 등 처리할 것들은 여기다가 추가 하면 된다

	inline static MysqlDatabase* ms_pInstance = nullptr;

};