/*
 *	작성자 : 윤정도
 *	생성일 : 2026/03/07
 *
 *	SQL Server ODBC 기반 데이터베이스 클래스
 *	IDatabase 인터페이스를 구현한다.
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
 *	auto stmt = PreparedStatement(SqlServerStatementBuilder::Build("insert into t_account (c_id, c_pass) values (?, ?)", id, id));
 *	auto spQueryTask = sg::GameDB->QueryAsync(stmt);
 *	auto& queryResult = spQueryTask->Wait();
 *
 *	auto stmt2 = PreparedStatement(SqlServerStatementBuilder::Build(
 *		"select c_account_id, c_id, c_pass, c_created, c_logined from t_account"
 *	));
 *	IQueryPtr q = sg::GameDB->Query(stmt2);
 */

#pragma once

#include <jdb/IDatabase.h>
#include <jdb/ConnectionPool.h>

#include "SqlServerConnection.h"

NS_JDB_BEGIN

class JDB_DLL SqlServerDatabase : public IDatabase
{
public:
	SqlServerDatabase();
	~SqlServerDatabase() override;

	SqlServerConnection* BeginTransaction();
	bool                 CommitTransaction(SqlServerConnection* _pConn);
	bool                 RollbackTransaction(SqlServerConnection* _pConn);
	IQueryPtr            QueryOnConnection(SqlServerConnection* _pConn, const Ptmt& _stmt);

protected:
	IQueryPtr	CreateQuery(IConnection* _pConn, const Ptmt& _ptmt) const override;
	int			GetTaskType() const override { return IOCP_TASK_TYPE_SQL_SERVER; }
	_u32		GetQueryFailedErrorCode() const override { return IOCPTASK_FAILED_SQLSERVER; }

private:

	DatabaseInfo info_{};
	bool initialized_;
};

NS_END
