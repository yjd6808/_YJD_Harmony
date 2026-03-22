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
IQueryPtr MysqlDatabase::CreateQuery(IConnection* _pConn, const BoundStmt& _stmt) const
{
	const String& ptmtStr = _stmt.text_;
	if (ptmtStr.IsEmpty())
		return nullptr;

	IQueryPtr pQuery;
	switch (_stmt.info_.type_)
	{
	case StatementType::Select: pQuery = MakeShared<MysqlQuerySelect>(); break;
	case StatementType::Update: pQuery = MakeShared<MysqlQueryUpdate>(); break;
	case StatementType::Delete: pQuery = MakeShared<MysqlQueryDelete>(); break;
	case StatementType::Insert: pQuery = MakeShared<MysqlQueryInsert>(); break;
	default:
		jc_assert_msg(false, "올바르지 않은 스테이트먼트입니다.");
		return nullptr;
	}

	pQuery->Init(_pConn, ptmtStr);
	return pQuery;
}
