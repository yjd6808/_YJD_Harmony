/*
	생성일 : 2019/03/06
	수정일 : 2022/04/03 - 대거 수정함
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

class MysqlQuery;



class MysqlQueryResult;
class MysqlQuery
{
	using TResultRow = std::map<int, JCore::String>;


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

	unsigned int GetResultRowCount();
	unsigned int GetFieldCount();

	bool IsSuccess() const { return m_bSuccess; }
	bool IsFailed() const { return !m_bSuccess; }


	MysqlQueryResult Result();
private:
	MysqlConnection* m_sqlConn;
	JCore::String m_PreparedStatement;
	bool m_bSuccess;
	std::map<int, TResultRow> m_ResultMap;
	std::map<int, JCore::String> m_FieldMap;
	std::map<JCore::String, int> m_FieldStringToIntMap;
};


// MysqlQuery가 실행, 결과 모두 보여주기땜에 결과만 보여주는 녀석 추가
// MysqlQuery의 ExecuteQuery, ExecuteUpdate 로 실행 후에 이 녀석을 얻어서 쓰면 편함
class MysqlQueryResult
{
public:
	MysqlQueryResult(MysqlQuery* ref, bool success) : m_pQuery(ref), m_bSuccess(success) {}

	bool IsSuccess() const { return m_bSuccess; }
	bool IsFailed() const { return !m_bSuccess; }

	unsigned int GetResultRowCount();
	unsigned int GetFieldCount();
	const JCore::String GetFieldName(const unsigned int& field) { return m_pQuery->GetFieldName(field);  }
	const JCore::String GetString(const unsigned int& row, const unsigned int& field) { return m_pQuery->GetString(row, field); }
	const JCore::String GetString(const unsigned int& row, const JCore::String& field) { return m_pQuery->GetString(row, field); }
	int GetInt(const unsigned int& row, const unsigned int& field) { return m_pQuery->GetInt(row, field); }
	int GetInt(const unsigned int& row, const JCore::String& field) { return m_pQuery->GetInt(row, field); }
	double GetDouble(const unsigned int& row, const unsigned int& field) { return m_pQuery->GetDouble(row, field); }
	double GetDouble(const unsigned int& row, const JCore::String& field) { return m_pQuery->GetDouble(row, field); }
	JCore::DateTime GetTime(const unsigned int& row, const unsigned int& field) { return  m_pQuery->GetTime(row, field); }
	JCore::DateTime GetTime(const unsigned int& row, const JCore::String& field) { return  m_pQuery->GetTime(row, field); }
private:
	MysqlQuery* m_pQuery;
	bool m_bSuccess;
};

