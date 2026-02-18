/*
 * 작성자: 윤정도
 */
#pragma once

#include <jnet/Namespace.h>

NS_JDB_BEGIN

enum class StatementType
{
	None,
	Select,
	Update,
	Delete,
	Insert
};

constexpr const char* StatementName(StatementType _statementType)
{
	if (_statementType == StatementType::Select) return "Select";
	if (_statementType == StatementType::Update) return "Update";
	if (_statementType == StatementType::Delete) return "Delete";
	if (_statementType == StatementType::Insert) return "Insert";
	return "None";
}

NS_END