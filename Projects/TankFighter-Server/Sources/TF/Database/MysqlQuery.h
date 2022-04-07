/*
	������ : 2019/03/06
	������ : 2022/04/03 - ��� ������
 */


#pragma once


#include <algorithm>
#include <string>
#include <map>
#include <sstream>

#include <TF/Database/MysqlConnection.h>
#include <JCore/Time.h>
#include <TF/Database/MysqlStatementBuilder.h>
#include <TF/Database/StatementType.h>

#ifndef DebugAssert
	#include <cassert>
	#define DebugAssert(exp, msg)		assert((exp) && msg)
#endif

class MysqlQuery
{
	using TResultRow = std::map<int, JCore::String>;
public:
	MysqlQuery(MysqlConnection *mConn, const JCore::String& preparedStatement);
	~MysqlQuery();

	// �� 6���ڸ� Ȯ���ؼ� � Ÿ���� Statement���� Ȯ��
	// ���ڿ��� select�� �����ϸ� SELECT Ÿ�� - �� �̷���
	static StatementType ParseStatement(const JCore::String& statement);

	// ����鼭 ���ÿ� ���ڸ� �־��ֵ���...
	template <typename... Args>
	static MysqlQuery* Create(MysqlConnection* conn, const JCore::String& statement, Args&&... args) {
		JCore::String preparedStatement = MysqlStatementBuilder::Build(statement, JCore::Forward<Args>(args)...);

		if (preparedStatement == "")
			return nullptr;

		return new MysqlQuery(conn, preparedStatement);
	}

	
	// SELECT
	bool ExecuteQuery();

	// DELETE | UDDATE ��
	bool ExecuteUpdate();

	// INSERT ������ ������ ID�� ����
	int ExecuteInsert();

	// ������Ʈ��Ʈ �� 6���ڸ� �Ľ��ؼ� select, delete � �°� �˾Ƽ� ����
	bool ExecuteAuto();

	JCore::String GetFieldName(const unsigned int& field);
	JCore::String GetString(const unsigned int& rowIdx, const unsigned int& fieldIdx);
	JCore::String GetString(const unsigned int& rowIdx, const JCore::String& fieldName);
	int GetInt(const unsigned int &rowIdx, const unsigned int &fieldIdx);
	int GetInt(const unsigned int &rowIdx, const JCore::String &fieldName);
	double GetDouble(const unsigned int &rowIdx, const unsigned int &fieldIdx);
	double GetDouble(const unsigned int &rowIdx, const JCore::String &fieldName);
	JCore::DateTime GetTime(const unsigned int &rowIdx, const unsigned int &fieldIdx);
	JCore::DateTime GetTime(const unsigned int &rowIdx, const JCore::String &fieldName);

	unsigned int GetResultRowCount();
	unsigned int GetFieldCount();

	bool IsSuccess() const { return m_bSuccess; }
	bool IsFailed() const { return !m_bSuccess; }

private:
	MysqlConnection* m_sqlConn;
	JCore::String m_PreparedStatement;
	bool m_bSuccess;
	std::map<int, TResultRow> m_ResultMap;
	std::map<int, JCore::String> m_FieldMap;
	std::map<JCore::String, int> m_FieldStringToIntMap;
};

