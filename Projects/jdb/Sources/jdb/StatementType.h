/*
 * 작성자: 윤정도
 */
#pragma once

#include "jnet/Namespace.h"

NS_JDB_BEGIN

enum class StatementType
{
	None,
	Select,
	Update,
	Delete,
	Insert,
	Etc,
};

constexpr const _char* StatementName(StatementType _statementType)
{
	if (_statementType == StatementType::Select) return _T("Select");
	if (_statementType == StatementType::Update) return _T("Update");
	if (_statementType == StatementType::Delete) return _T("Delete");
	if (_statementType == StatementType::Insert) return _T("Insert");
	return _T("None");
}

NS_END