/*
	생성일 : 2019/03/06
	수정일 : 2022/04/03 - 대거 수정함
	
	이게 옛날에 깃허브에서 검색 도중 발견했던건데 대거 수정함
	오류도 좀 있었고 다 뜯어고침
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

	// 앞 6글자를 확인해서 어떤 타입의 Statement인지 확인
	// 문자열이 select로 시작하면 SELECT 타입 - 뭐 이런식
	static StatementType ParseStatement(const JCore::String& statement);

	// 만들면서 동시에 인자를 넣어주도록...
	template <typename... Args>
	static MysqlQuery* Create(MysqlConnection* conn, const JCore::String& statement, Args&&... args) {
		JCore::String preparedStatement = MysqlStatementBuilder::Build(statement, JCore::Forward<Args>(args)...);

		if (preparedStatement == "")
			return nullptr;

		return new MysqlQuery(conn, preparedStatement);
	}

	
	// SELECT
	bool ExecuteQuery();

	// DELETE | UDDATE 용
	bool ExecuteUpdate();

	// INSERT 실행후 마지막 ID값 얻음
	int ExecuteInsert();

	// 스테이트먼트 앞 6글자를 파싱해서 select, delete 등에 맞게 알아서 실행
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

