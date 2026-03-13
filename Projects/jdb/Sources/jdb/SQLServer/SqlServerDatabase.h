/*
 *	작성자 : 윤정도
 *	생성일 : 2026/03/07
 *
 *	SQL Server ODBC 기반 데이터베이스 클래스
 *	MySQL 데이터베이스 클래스와 동일한 인터페이스를 제공한다.
 *
 *	사용 예시코드:
 *
 *	-- 계정 테이블
 *	CREATE TABLE t_account(
 *		c_account_id				INT				IDENTITY(1,1) PRIMARY KEY,
 *		c_id						NVARCHAR(20)	UNIQUE,
 *		c_pass						NVARCHAR(20)	NOT NULL,
 *		c_created					DATETIME2		DEFAULT SYSDATETIME(),
 *		c_logined					DATETIME2		DEFAULT NULL
 *	);
 *
 *	char id[] = "wjdeh818";
 *	auto spQueryTask = sg::GameDB->QueryAsync("insert into t_account (c_id, c_pass) values (?, ?)", id, id);
 *	auto& queryResult = spQueryTask->Wait();
 *
 *	SharedPtr<SqlServerQuerySelect> q = sg::GameDB->Query(
 *		"select c_account_id, c_id, c_pass, c_created, c_logined from t_account"
 *	);
 *
 *	int iFieldCount = q->GetFieldCount();
 *	int iRowCount = q->GetRowCount();
 *
 *	if (!q->HasNext())
 *		return;
 *
 *	do {
 *		auto c1 = q->GetNumber<_s32>("c_account_id");
 *		auto c2 = q->GetString("c_id");
 *		auto c3 = q->GetString("c_pass");
 *		auto c4 = q->GetDateTime("c_created");
 *	} while (q->Next());
 */

#pragma once

#include <jnet/IOCP/IOCPTask.h>
#include <jnet/IOCP/IOCPTaskListener.h>
#include <jdb/Structure.h>

#include "SqlServerConnectionPool.h"
#include "SqlServerStatementBuilder.h"
#include "SqlServerQuery.h"

#define IOCP_TASK_TYPE_SQL_SERVER (2)

NS_JDB_BEGIN

using SqlServerQueryTask = jnet::IOCPTask<SqlServerQueryPtr>;
using SqlServerQueryTaskPtr = jc::SharedPtr<SqlServerQueryTask>;

#define IOCPTASK_FAILED_SQLSERVER	50002

class JDB_DLL SqlServerDatabase
{
public:
	SqlServerDatabase();
	virtual ~SqlServerDatabase();

	bool Initialize(const SqlServerDatabaseInfo& _info);
	void Finalize();

	SqlServerConnectionPool* GetConnectionPool() const { return connectionPool_; }

	void SetListener(const jnet::IOCPTaskListenerPtr& _pListener);
	void SetListener(jnet::IOCPTaskListenerPtr&& _pListener);

	void SetOnTaskCompletedCallback(const jnet::IOCPTaskListener::FnOnTaskCompleted& _fn);
	void SetOnTaskCompletedCallback(jnet::IOCPTaskListener::FnOnTaskCompleted&& _fn);

	int PollEvents();

	/*
	 * 비동기 Query 실행
	*/
	template <typename... Args>
	SqlServerQueryTaskPtr QueryAsync(const jc::String& _statement, Args&&... _args)
	{
		return QueryAsyncInternal(0, _statement, jc::Forward<Args>(_args)...);
	}

	template <typename... Args>
	SqlServerQueryTaskPtr QueryAsync(int _id, const jc::String& _statement, Args&&... _args)
	{
		return QueryAsyncInternal(_id, _statement, jc::Forward<Args>(_args)...);
	}

	/*
	 * 동기화 Query 실행
	 */
	template <typename... Args>
	SqlServerQueryPtr Query(const jc::String& _statement, Args&&... _args)
	{
		if (connectionPool_ == nullptr)
		{
			jc_assert_msg(false, "커넥션 풀이 초기화되지 않았습니다. 데이터베이스가 연결되어있는지 확인해주세요.");
			return nullptr;
		}

		auto pConn = connectionPool_->GetConnection();
		SqlServerAutoReleaseConnection autoRelease(pConn, connectionPool_);

		if (pConn == nullptr)
		{
			jc_assert_msg(false, "SqlServerDatabase::Query() 커넥션 풀에서 가져오기 실패");
			return nullptr;
		}

		SqlServerQueryPtr pQuery = SqlServerQuery::Create(pConn, _statement, jc::Forward<Args>(_args)...);

		if (pQuery == nullptr)
		{
			jc_assert_msg(false, "SqlServerDatabase::Query() 쿼리문 파싱 실패");
			return nullptr;
		}

		pQuery->Execute();
		return pQuery;
	}

	/*
	 * 트랜잭션 제어
	 * 커넥션 풀에서 커넥션을 가져와서 트랜잭션을 시작하고,
	 * 완료 후 반환할 수 있도록 한다.
	 */
	SqlServerConnection* BeginTransaction();
	bool CommitTransaction(SqlServerConnection* _pConn);
	bool RollbackTransaction(SqlServerConnection* _pConn);

	// 트랜잭션 내에서 쿼리 실행 (특정 커넥션 사용)
	template <typename... Args>
	SqlServerQueryPtr QueryOnConnection(SqlServerConnection* _pConn, const jc::String& _statement, Args&&... _args)
	{
		if (_pConn == nullptr)
		{
			jc_assert_msg(false, "SqlServerDatabase::QueryOnConnection() 커넥션이 NULL입니다.");
			return nullptr;
		}

		SqlServerQueryPtr pQuery = SqlServerQuery::Create(_pConn, _statement, jc::Forward<Args>(_args)...);

		if (pQuery == nullptr)
		{
			jc_assert_msg(false, "SqlServerDatabase::QueryOnConnection() 쿼리문 파싱 실패");
			return nullptr;
		}

		pQuery->Execute();
		return pQuery;
	}

private:
	template <typename... Args>
	SqlServerQueryTaskPtr QueryAsyncInternal(int _id, const jc::String& _statement, Args&&... _args)
	{
		if (connectionPool_ == nullptr)
		{
			jc_assert_msg(false, "커넥션 풀이 초기화되지 않았습니다. 데이터베이스가 연결되어있는지 확인해주세요.");
			return nullptr;
		}

		const auto& taskFunc = [this](SqlServerQueryTask::TResult& _result)
			{
				_result.success_ = _result.value_->Execute();
				_result.errorCode_ = !_result.success_ ? IOCPTASK_FAILED_SQLSERVER : 0;
			};

		const auto& finallyFunc = [this](SqlServerQueryTask::TResult& _result)
			{
				connectionPool_->ReleaseConnection(_result.value_->GetConnection());
			};

		SqlServerQueryPtr pQuery = SqlServerQuery::Create(connectionPool_->GetConnection(), _statement, jc::Forward<Args>(_args)...);
		auto pTask = SqlServerQueryTask::Create(iocp_, taskFunc, finallyFunc, pQuery);
		pTask->SetId(_id);
		pTask->SetType(IOCP_TASK_TYPE_SQL_SERVER);
		pTask->Start();
		return pTask;
	}

	jnet::IOCP* iocp_;
	SqlServerConnectionPool* connectionPool_;
	SqlServerDatabaseInfo info_{};
	bool initialized_;
};

NS_END
