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
#include <jdb/Util.h>

#include <jdb/MySQL/MysqlStatementBuilder.h>
#include <jdb/SQLServer/SqlServerStatementBuilder.h>

NS_JDB_BEGIN

class JDB_DLL Ptmt
{
public:
	Ptmt() = default;
	explicit Ptmt(const jc::String& _statement) : statement_(_statement) {}
	explicit Ptmt(jc::String&& _statement) : statement_(jc::Move(_statement)) {}

	const jc::String& GetStatement() const { return statement_; }
	bool IsEmpty() const { return statement_.IsEmpty(); }

	template <typename... Args>
	static Ptmt Build(DatabaseType _dbType, const jc::String& _stmt, Args&&... _args)
	{
		jc::String ptmt(0);
		if constexpr (sizeof...(Args) == 0)
		{
			ptmt = _stmt;
		}
		else
		{
			if (_dbType == DatabaseType::dbtMySQL)
			{
				ptmt = BuildInternal<MysqlStatementBuilder, Args...>(_stmt, jc::Forward<Args>(_args)...);
			}
			else
			{
				ptmt = BuildInternal<SqlServerStatementBuilder, Args...>(_stmt, jc::Forward<Args>(_args)...);
			}
		}
		return Ptmt(jc::Move(ptmt));
	}

private:
	template <typename TBuilder, typename T, typename... Args>
	constexpr static jc::String BuildInternal(const jc::String& _stmt, T&& _arg, Args&&... _args)
	{
		if (!TBuilder::IsInitialized())
		{
			jc_assert_msg(false, "우선 빌더의 Initialize를 호출해주세요");
			return jc::String();
		}

		constexpr int ARG_COUNT = sizeof...(Args) + 1;
		static_assert(ARG_COUNT <= Util::MAX_ARGS, "인자 수가 MAX_ARGS를 초과합니다.");

		thread_local jc::String fieldMap[Util::MAX_ARGS];
		TBuilder::Set(fieldMap, 0, jc::Forward<T>(_arg), jc::Forward<Args>(_args)...);
		return jc::String(Util::ReplacePlaceholders(_stmt, fieldMap, ARG_COUNT));
	}

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

	bool				IsPollingMode() const { return iocp_ ? iocp_->IsPollingMode() : false; }
	const DatabaseInfo& GetDatabaseInfo() const { return info_; }
	ConnectionPool*		GetConnectionPool() const { return connectionPool_; }
	DatabaseType		GetType() const { return info_.type_; }
	int					GetId() const { return info_.id_; }
	int					GetPendingQueryCount() const { return iocp_ ? iocp_->GetPendingCount() : 0; }

	void				SetQueryCompletedCallback(const FnQueryCompleted& _fn) { fnQueryCompleted_ = _fn; }
	void				SetQueryCompletedCallback(FnQueryCompleted&& _fn) { fnQueryCompleted_ = jc::Move(_fn); }

	IQueryPtr     Query(const Ptmt& _ptmt) const;
	IQueryPtr     Query(int _id, const Ptmt& _ptmt) const;
	IQueryTaskPtr QueryAsync(const Ptmt& _ptmt) const;
	IQueryTaskPtr QueryAsync(int _id, const Ptmt& _ptmt) const;


	template <typename... Args>
	IQueryPtr Query(const jc::String& _stmt, Args&&... _args)
	{
		Ptmt ptmt = Ptmt::Build(info_.type_, _stmt, jc::Forward<Args>(_args)...);
		return Query(ptmt);
	}

	template <typename... Args>
	IQueryPtr Query(int _id, const jc::String& _stmt, Args&&... _args)
	{
		Ptmt ptmt = Ptmt::Build(info_.type_, _stmt, jc::Forward<Args>(_args)...);
		return Query(_id, ptmt);
	}

	template <typename... Args>
	IQueryTaskPtr QueryAsync(const jc::String& _stmt, Args&&... _args)
	{
		Ptmt ptmt = Ptmt::Build(info_.type_, _stmt, jc::Forward<Args>(_args)...);
		return QueryAsync(ptmt);
	}

	template <typename... Args>
	IQueryTaskPtr QueryAsync(int _id, const jc::String& _stmt, Args&&... _args)
	{
		Ptmt ptmt = Ptmt::Build(info_.type_, _stmt, jc::Forward<Args>(_args)...);
		return QueryAsync(_id, ptmt);
	}

protected:
	virtual IQueryPtr	CreateQuery(IConnection* _pConn, const Ptmt& _stmt) const = 0;
	virtual int			GetTaskType() const = 0;
	virtual _u32		GetQueryFailedErrorCode() const = 0;
	IQueryTaskPtr		QueryAsyncInternal(int _id, const Ptmt& _stmt) const;

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
