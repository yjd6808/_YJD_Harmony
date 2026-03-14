#include "Core.h"
#include "MysqlDatabase.h"

#include "MysqlQuery.h"
#include "MysqlConnection.h"
#include "MysqlStatementBuilder.h"

USING_NS_JC;
USING_NS_JNET;
USING_NS_STD;
USING_NS_JDB;


//////////////////////////////////////////////////////////////////////////////////////////
MysqlDatabase::MysqlDatabase()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
MysqlDatabase::~MysqlDatabase()
{
	if (initialized_)
	{
		MysqlDatabase::Finalize();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
IQueryPtr MysqlDatabase::CreateQuery(IConnection* _pConn, const PreparedStatement& _stmt) const
{
	const String& stmtStr = _stmt.GetStatement();
	if (stmtStr.IsEmpty())
		return nullptr;

	const StatementType type = IQuery::ParseStatement(stmtStr);

	IQueryPtr pQuery;
	switch (type)
	{
	case StatementType::Select: pQuery = MakeShared<MysqlQuerySelect>(); break;
	case StatementType::Update: pQuery = MakeShared<MysqlQueryUpdate>(); break;
	case StatementType::Delete: pQuery = MakeShared<MysqlQueryDelete>(); break;
	case StatementType::Insert: pQuery = MakeShared<MysqlQueryInsert>(); break;
	default:
		jc_assert_msg(false, "올바르지 않은 스테이트먼트입니다.");
		return nullptr;
	}

	pQuery->pConn_             = _pConn;
	pQuery->stmtType_          = type;
	pQuery->ptmt_ = stmtStr;
	return pQuery;
}
