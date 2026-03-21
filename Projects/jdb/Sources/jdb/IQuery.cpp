/*
 * 작성자: 윤정도
 * 생성일: 3/13/2026
 * =====================
 *
 * IQuery 공통 구현
 */

#include "Core.h"
#include "IQuery.h"

USING_NS_JC;

NS_JDB_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
void IQuery::Init(IConnection* _pConn, const jc::String& _ptmt)
{
	pConn_ = _pConn;
	ptmt_ = _ptmt;
}

//////////////////////////////////////////////////////////////////////////////////////////
StatementType IQuery::ParseStatement(const String& _statement)
{
	const String statementPrefix = _statement.GetRange(0, 5).ToLowerCase();

	if (statementPrefix.Find("select") == 0)
		return StatementType::Select;
	if (statementPrefix.Find("update") == 0)
		return StatementType::Update;
	if (statementPrefix.Find("insert") == 0)
		return StatementType::Insert;
	if (statementPrefix.Find("delete") == 0)
		return StatementType::Delete;

	jc_assert_msg(false, "올바른 스테이트먼트가 아닙니다.");
	return StatementType::None;
}

NS_END
