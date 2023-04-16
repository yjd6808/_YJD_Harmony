/*	
 *	작성자 : 윤정도
 *	사용 에시코드
 *
 *	--계정 테이블
 *	create table t_account(
 *		c_account_id				int				primary key auto_increment			comment '계정 PK',
 *		c_id						varchar(20) 	unique								comment '계정 아이디',
 *		c_pass						varchar(20)		not null							comment '계정 비번',
 *		c_created					timestamp		default current_timestamp			comment '계정 생성날짜',
 *		c_created2					timestamp(3)	default current_timestamp(3)		comment '계정 생성날짜',
 *		c_logined					timestamp		default null						comment '계정 접속날짜'
 *	);
 *	
 *	char id[] = "wjdeh818";
 *	auto spQueryTask = CoreGameDB_v->QueryAsync("insert into t_account (c_id, c_pass) values (?, ?)", id, id);
 *	auto& queryResult = spQueryTask->Wait();
 *	
 *	SharedPtr<MysqlQuerySelect> q = CoreGameDB_v->Query(
 *		"select c_account_id, c_id, c_pass, unix_timestamp(c_created) as c_created, unix_timestamp(c_created2) as c_created2, unix_timestamp(c_logined) as c_logined from t_account"
 *	);
 *	
 *	int iFieldCount = q->GetFieldCount();
 *	int iRowCount = q->GetRowCount();
 *
 *	if (!q->HasNext())
 *		return;
 *
 *	do {
 *		auto c1 = q->GetNumber<Int32>("c_account_id");
 *		auto c2 = q->GetString("c_id");
 *		auto c3 = q->GetString("c_pass");
 *		auto c4 = q->GetNumber<double>("c_created2");
 *	
 *		DateTime dtCreated = DateTime::FromUnixTime(c4).FormatMysqlTime();
 *	} while (q->Next());
 */

#pragma once

#include <JNetwork/IOCP/IOCPTask.h>

#include <SteinsGate/Common/MysqlConnectionPool.h>
#include <SteinsGate/Common/MysqlStatementBuilder.h>
#include <SteinsGate/Common/MysqlQuery.h>
#include <SteinsGate/Common/DatabaseInfo.h>

using MysqlQueryTask = JNetwork::IOCPTask<MysqlQueryPtr>;
using MysqlQueryTaskPtr = JCore::SharedPtr<MysqlQueryTask>;

#define IOCPTASK_FAILED_DB	50001

class MysqlDatabase
{
public:
	MysqlDatabase(DatabaseInfo* info);
	virtual ~MysqlDatabase();
	
	bool Initialize(ServerProcessType_t serverProcessType);
	void Finalize();

	MysqlConnectionPool* GetConnectionPool() const { return m_pConnectionPool; }

	/*
	 * 비동기 Query 실행
	*/
	template <typename... Args>
	MysqlQueryTaskPtr QueryAsync(const JCore::String& statement, Args&&... args) {
		const auto& fnTask = [this](MysqlQueryTask::TResult& result) {
			result.Success = result.Value->Execute();
			result.ErrorCode = !result.Success ? IOCPTASK_FAILED_DB : 0;
		};

		const auto& fnFinally = [this](MysqlQueryTask::TResult& result) {
			m_pConnectionPool->ReleaseConnection(result.Value->GetConnection());
		};

		MysqlQueryPtr spQuery = MysqlQuery::Create(m_pConnectionPool->GetConnection(), statement, JCore::Forward<Args>(args)...);
		return MysqlQueryTask::Run(m_pIocp, fnTask, fnFinally, spQuery);
	}

	/*
	 * 동기화 Query 실행
	 * 스마트 포인터로 알아서 해제해주므로 상관없다.
	 */

	template <typename... Args>
	MysqlQueryPtr Query(const JCore::String& statement, Args&&... args) {
		auto pConn = m_pConnectionPool->GetConnection();
		AutoReleaseConnection autoRelease(pConn, m_pConnectionPool);

		if (pConn == nullptr) {
			// 실패
			DebugAssertMsg(false, "MysqlDatabase::Query() 커넥션 풀에서 가져오기 실패");
			return nullptr;
		}

		MysqlQueryPtr spQuery = MysqlQuery::Create(pConn, statement, JCore::Forward<Args>(args)...);
		spQuery->Execute();
		return spQuery;
	}
private:
	
	JNetwork::IOCP* m_pIocp;
	MysqlConnectionPool* m_pConnectionPool;
	DatabaseInfo* m_pInfo{};
	bool m_bFinalized;

	// 쿼리 수행 통계
	// 실패 등 처리할 것들은 여기다가 추가 하면 된다
};