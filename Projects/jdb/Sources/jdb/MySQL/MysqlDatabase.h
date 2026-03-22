/*
 *	작성자 : 윤정도
 *	사용 예시코드
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
 *	auto stmt = PreparedStatement(MysqlStatementBuilder::Build("insert into t_account (c_id, c_pass) values (?, ?)", id, id));
 *	auto spQueryTask = sg::GameDB->QueryAsync(stmt);
 *	auto& queryResult = spQueryTask->Wait();
 *	
 *	auto stmt2 = PreparedStatement(MysqlStatementBuilder::Build(
 *		"select c_account_id, c_id, c_pass, unix_timestamp(c_created) as c_created from t_account"
 *	));
 *	SharedPtr<IQuery> q = sg::GameDB->Query(stmt2);
 *	
 *	int iFieldCount = q->GetFieldCount();
 *	int iRowCount = q->GetRowCount();
 */

#pragma once

#include "jdb/IDatabase.h"
#include "jdb/ConnectionPool.h"

NS_JDB_BEGIN

class JDB_DLL MysqlDatabase : public IDatabase
{
public:
	MysqlDatabase();
	~MysqlDatabase() override;

protected:
	virtual IQueryPtr CreateQuery(IConnection* _pConn, const BoundStmt& _stmt) const override;
	virtual int GetTaskType() const override { return IOCP_TASK_TYPE_MYSQL; }
	virtual _u32 GetQueryFailedErrorCode() const override { return IOCPTASK_FAILED_DB; }

};

NS_END