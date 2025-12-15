/*
 *	작성자 : 윤정도
 *	사용 에시코드
 *
 *	--계정 테이블
 *	create table t_account(
 *		c_account_id				int				primary key auto_increment			comment '계정 PK',
 *		c_id						varchar(20)		unique							comment '계정 아이디',
 *		c_pass						varchar(20)		not null						comment '계정 비번',
 *		c_created					timestamp		default current_timestamp			comment '계정 생성날짜',
 *		c_created2					timestamp(3)	default current_timestamp(3)			comment '계정 생성날짜',
 *		c_logined					timestamp		default null					comment '계정 접속날짜'
 *	);
 *	
 *	char id[] = "wjdeh818";
 *	auto spQueryTask = Core::GameDB->QueryAsync("insert into t_account (c_id, c_pass) values (?, ?)", id, id);
 *	auto& queryResult = spQueryTask->Wait();
 *	
 *	SharedPtr<MysqlQuerySelect> q = Core::GameDB->Query(
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
	MysqlDatabase(DatabaseInfo* _pInfo);
	virtual ~MysqlDatabase();

	bool Initialize(ServerProcessType_t _serverProcessType);
	void Finalize();

	MysqlConnectionPool* GetConnectionPool() const { return connectionPool_; }

	/*
	 * 비동기 Query 실행
	*/
	template <typename... Args>
	MysqlQueryTaskPtr QueryAsync(const JCore::String& _statement, Args&&... _args)
	{
		if (connectionPool_ == nullptr)
		{
			DebugAssertMsg(false, "커넥션 풀이 초기화되지 않았습니다. 데이터베이스가 연결되어있는지 확인해주세요.");
			return nullptr;
		}

		const auto& taskFunc = [this](MysqlQueryTask::TResult& _result)
		{
			_result.success_ = _result.value_->Execute();
			_result.errorCode_ = !_result.success_ ? IOCPTASK_FAILED_DB : 0;
		};

		const auto& finallyFunc = [this](MysqlQueryTask::TResult& _result)
		{
			connectionPool_->ReleaseConnection(_result.value_->GetConnection());
		};

		MysqlQueryPtr pQuery = MysqlQuery::Create(connectionPool_->GetConnection(), _statement,
		                                          JCore::Forward<Args>(_args)...);
		return MysqlQueryTask::Run(iocp_, taskFunc, finallyFunc, pQuery);
	}

	/*
	 * 동기화 Query 실행
	 * 스마트 포인터로 알아서 해제해주므로 상관없다.
	 */

	template <typename... Args>
	MysqlQueryPtr Query(const JCore::String& _statement, Args&&... _args)
	{
		if (connectionPool_ == nullptr)
		{
			DebugAssertMsg(false, "커넥션 풀이 초기화되지 않았습니다. 데이터베이스가 연결되어있는지 확인해주세요.");
			return nullptr;
		}

		auto pConn = connectionPool_->GetConnection();
		AutoReleaseConnection autoRelease(pConn, connectionPool_);

		if (pConn == nullptr)
		{
			// 실패
			DebugAssertMsg(false, "MysqlDatabase::Query() 커넥션 풀에서 가져오기 실패");
			return nullptr;
		}

		MysqlQueryPtr pQuery = MysqlQuery::Create(pConn, _statement, JCore::Forward<Args>(_args)...);

		if (pQuery == nullptr)
		{
			DebugAssertMsg(false, "MysqlDatabase::Query() 쿼리문 파싱 실패");
			return nullptr;
		}

		pQuery->Execute();
		return pQuery;
	}

private:
	JNetwork::IOCP* iocp_;
	MysqlConnectionPool* connectionPool_;
	DatabaseInfo* info_{};
	bool initialized_;

	// 쿼리 수행 통계
	// 실패 등 처리할 것들은 여기다가 추가 하면 된다
};
