/*
	������ : 2019/03/06
	������ : 2022/04/03 - ��� ������
	
	�̰� ������ ����꿡�� �˻� ���� �߰��ߴ��ǵ� ��� ������
	������ �� �־��� �� ����ħ
 */


#pragma once


#include <algorithm>
#include <string>
#include <map>
#include <sstream>

#include <JCore/Container/HashMap.h>
#include <JCore/Time.h>

#include <TF/Database/MysqlConnection.h>

#include <TF/Database/MysqlStatementBuilder.h>
#include <TF/Database/StatementType.h>

#ifndef DebugAssert
	#include <cassert>
	#define DebugAssert(exp, msg)		assert((exp) && msg)
#endif

class MysqlQuery
{
	using TResultRow = JCore::HashMap<int, JCore::String>;
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

	unsigned int GetResultRowCount() const;
	unsigned int GetFieldCount() const;

	bool IsSuccess() const { return m_bSuccess; }
	bool IsFailed() const { return !m_bSuccess; }

private:
	MysqlConnection* m_sqlConn;
	JCore::String m_PreparedStatement;
	bool m_bSuccess;
	JCore::HashMap<int, TResultRow> m_ResultMap;
	JCore::HashMap<int, JCore::String> m_FieldMap;
	JCore::HashMap<JCore::String, int> m_FieldStringToIntMap;
};

