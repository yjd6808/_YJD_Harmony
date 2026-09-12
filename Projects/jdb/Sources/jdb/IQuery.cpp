/*
 * 작성자: 윤정도
 * 생성일: 3/13/2026
 * =====================
 *
 * IQuery 공통 구현
 */

#include "IQuery.h"

USING_NS_JC;

NS_JDB_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
void IQuery::Init(IConnection* _pConn, const jc::String& _stmt)
{
	pConn_ = _pConn;
	ptmt_ = _stmt;
}

//////////////////////////////////////////////////////////////////////////////////////////
StatementType IQuery::ParseStatement(const String& _statement)
{
	const String statementPrefix = _statement.GetRange(0, 5).ToLowerCase();

	if (statementPrefix.Find(_T("select")) == 0)
		return StatementType::Select;
	if (statementPrefix.Find(_T("update")) == 0)
		return StatementType::Update;
	if (statementPrefix.Find(_T("insert")) == 0)
		return StatementType::Insert;
	if (statementPrefix.Find(_T("delete")) == 0)
		return StatementType::Delete;

	jc_assert_msg(false, _T("올바른 스테이트먼트가 아닙니다."));
	return StatementType::None;
}

NS_END
