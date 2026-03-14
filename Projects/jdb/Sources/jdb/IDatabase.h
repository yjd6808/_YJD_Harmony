/*
 * 작성자: 윤정도
 * 생성일: 3/13/2026
 * =====================
 *
 * MySQL/SQLServer 공통 데이터베이스 추상 인터페이스
 * PreparedStatement 클래스를 통해 쿼리를 실행한다.
 */

#pragma once

#include <jc/Primitives/String.h>

#include <jnet/IOCP/IOCPTask.h>
#include <jnet/IOCP/IOCPTaskListener.h>

#include <jdb/Namespace.h>
#include <jdb/DLLExport.h>
#include <jdb/IQuery.h>
#include <jdb/Structure.h>
#include <jdb/JdbUtil.h>

#include <jdb/MySQL/MysqlStatementBuilder.h>
#include <jdb/SQLServer/SqlServerStatementBuilder.h>

NS_JDB_BEGIN

class JDB_DLL PreparedStatement
{
public:
	PreparedStatement() = default;
	explicit PreparedStatement(const jc::String& _statement) : statement_(_statement) {}
	explicit PreparedStatement(jc::String&& _statement) : statement_(jc::Move(_statement)) {}

	const jc::String& GetStatement() const { return statement_; }
	bool IsEmpty() const { return statement_.IsEmpty(); }

private:
	jc::String statement_;
};

using IQueryTask = jnet::IOCPTask<IQueryPtr>;
using IQueryTaskPtr = jc::SharedPtr<IQueryTask>;

#define IOCP_TASK_TYPE_MYSQL		(1)
#define IOCPTASK_FAILED_DB			50001

#define IOCP_TASK_TYPE_SQL_SERVER	(2)
#define IOCPTASK_FAILED_SQLSERVER	50002

class IConnection;
class ConnectionPool;
struct QueryCompletedCallbackFunctor;

class JDB_DLL IDatabase // 무조건 shared 포인터로 관리되어야 하는 인터페이스
{
public:
	using FnQueryCompleted = jc::Action<IQuery&>;

	virtual ~IDatabase() = default;
	virtual bool Initialize(const DatabaseInfo& _info);
	virtual void Finalize();

	int			 PollEvents();

	const DatabaseInfo& GetDatabaseInfo() const { return info_; }
	ConnectionPool*		GetConnectionPool() const { return connectionPool_; }
	DatabaseType		GetType() const { return info_.type_; }
	int					GetId() const { return info_.id_; }
	int					GetPendingQueryCount() const { return iocp_->GetPendingCount(); }

	void				SetQueryCompletedCallback(const FnQueryCompleted& _fn) { fnQueryCompleted_ = _fn; }
	void				SetQueryCompletedCallback(FnQueryCompleted&& _fn) { fnQueryCompleted_ = jc::Move(_fn); }

	IQueryPtr     Query(const PreparedStatement& _stmt) const;
	IQueryPtr     Query(int _id, const PreparedStatement& _stmt) const;
	IQueryTaskPtr QueryAsync(const PreparedStatement& _stmt) const;
	IQueryTaskPtr QueryAsync(int _id, const PreparedStatement& _stmt) const;

	template <typename... Args>
	IQueryTaskPtr QueryAsync(const jc::String& _statement, Args&&... _args)
	{
		PreparedStatement stmt = BuildStatement(_statement, jc::Forward<Args>(_args)...);
		return QueryAsync(stmt);
	}

	template <typename... Args>
	IQueryTaskPtr QueryAsync(int _id, const jc::String& _statement, Args&&... _args)
	{
		PreparedStatement stmt = BuildStatement(_statement, jc::Forward<Args>(_args)...);
		return QueryAsync(_id, stmt);
	}

	template <typename... Args>
	PreparedStatement BuildStatement(const jc::String& _statement, Args&&... _args) const
	{
		jc::String ptmt(0);
		if constexpr (sizeof...(Args) == 0)
		{
			ptmt = _statement;
		}
		else
		{
			if (info_.type_ == DatabaseType::MySQL)
			{
				ptmt = JdbUtil::Build<MysqlStatementBuilder, Args...>(_statement, jc::Forward<Args>(_args)...);
			}
			else
			{
				ptmt = JdbUtil::Build<SqlServerStatementBuilder, Args...>(_statement, jc::Forward<Args>(_args)...);
			}
		}
		return PreparedStatement(jc::Move(ptmt));
	}

protected:
	virtual IQueryPtr	CreateQuery(IConnection* _pConn, const PreparedStatement& _stmt) const = 0;
	virtual int			GetTaskType() const = 0;
	virtual _u32		GetQueryFailedErrorCode() const = 0;
	IQueryTaskPtr		QueryAsyncInternal(int _id, const PreparedStatement& _stmt) const;

	int				id_ = -1;
	DatabaseInfo	info_{};
	bool			initialized_	= false;
	jnet::IOCP*     iocp_            = nullptr;
	ConnectionPool* connectionPool_  = nullptr;
	FnQueryCompleted fnQueryCompleted_;

	friend struct QueryCompletedCallbackFunctor;
};

//////////////////////////////////////////////////////////////////////////////////////////
struct QueryCompletedCallbackFunctor
{
	QueryCompletedCallbackFunctor(IDatabase* _pDb) : pDb_(_pDb) {}

	IDatabase* pDb_;
	void operator()(jnet::IOCPTaskAbstract* _pTask);
};

using IDatabasePtr = jc::SharedPtr<IDatabase>;

NS_END
