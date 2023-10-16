/*
 * 작성자: 윤정도
 */
#pragma once

enum class StatementType
{
	None,
	Select,
	Update,
	Delete,
	Insert
};

constexpr const char* StatementName(StatementType stmt) {
	if (stmt == StatementType::Select) return "Select";
	if (stmt == StatementType::Update) return "Update";
	if (stmt == StatementType::Delete) return "Delete";
	if (stmt == StatementType::Insert) return "Insert";
	return "None";
}
