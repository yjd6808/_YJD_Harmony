#include "Core.h"
#include "SqlServerDatabase.h"

#include "SqlServerQuery.h"
#include "SqlServerConnection.h"
#include "SqlServerStatementBuilder.h"

USING_NS_JC;
USING_NS_JNET;
USING_NS_STD;
USING_NS_JDB;

//////////////////////////////////////////////////////////////////////////////////////////
SqlServerDatabase::SqlServerDatabase()
: initialized_(false)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
SqlServerDatabase::~SqlServerDatabase()
{
	if (initialized_)
	{
		SqlServerDatabase::Finalize();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
SqlServerConnection* SqlServerDatabase::BeginTransaction()
{
	if (connectionPool_ == nullptr)
	{
		jc_assert_msg(false, "커넥션 풀이 초기화되지 않았습니다.");
		return nullptr;
	}

	auto pConn = static_cast<SqlServerConnection*>(connectionPool_->GetConnection());
	if (pConn == nullptr)
	{
		jc_assert_msg(false, "SqlServerDatabase::BeginTransaction() 커넥션 풀에서 가져오기 실패");
		return nullptr;
	}

	if (!pConn->BeginTransaction())
	{
		connectionPool_->ReleaseConnection(pConn);
		return nullptr;
	}

	return pConn;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool SqlServerDatabase::CommitTransaction(SqlServerConnection* _pConn)
{
	if (_pConn == nullptr)
	{
		_LogError_("CommitTransaction() 실패 : 커넥션이 NULL입니다.");
		return false;
	}

	bool result = _pConn->Commit();
	connectionPool_->ReleaseConnection(_pConn);
	return result;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool SqlServerDatabase::RollbackTransaction(SqlServerConnection* _pConn)
{
	if (_pConn == nullptr)
	{
		_LogError_("RollbackTransaction() 실패 : 커넥션이 NULL입니다.");
		return false;
	}

	bool result = _pConn->Rollback();
	connectionPool_->ReleaseConnection(_pConn);
	return result;
}

//////////////////////////////////////////////////////////////////////////////////////////
IQueryPtr SqlServerDatabase::CreateQuery(IConnection* _pConn, const Ptmt& _ptmt) const
{
	const String& ptmtStr = _ptmt.GetStatement();
	if (ptmtStr.IsEmpty())
		return nullptr;

	const StatementType type = IQuery::ParseStatement(ptmtStr);

	IQueryPtr pQuery;
	switch (type)
	{
	case StatementType::Select: pQuery = MakeShared<SqlServerQuerySelect>(); break;
	case StatementType::Update: pQuery = MakeShared<SqlServerQueryUpdate>(); break;
	case StatementType::Delete: pQuery = MakeShared<SqlServerQueryDelete>(); break;
	case StatementType::Insert: pQuery = MakeShared<SqlServerQueryInsert>(); break;
	default:
		jc_assert_msg(false, "올바르지 않은 스테이트먼트입니다.");
		return nullptr;
	}

	pQuery->Init(_pConn, ptmtStr);
	return pQuery;
}

//////////////////////////////////////////////////////////////////////////////////////////
IQueryPtr SqlServerDatabase::QueryOnConnection(SqlServerConnection* _pConn, const Ptmt& _stmt)
{
	if (_pConn == nullptr)
	{
		jc_assert_msg(false, "SqlServerDatabase::QueryOnConnection() 커넥션이 NULL입니다.");
		return nullptr;
	}

	IQueryPtr pQuery = CreateQuery(_pConn, _stmt);

	if (pQuery == nullptr)
	{
		jc_assert_msg(false, "SqlServerDatabase::QueryOnConnection() 쿼리문 파싱 실패");
		return nullptr;
	}

	pQuery->Execute();
	return pQuery;
}
