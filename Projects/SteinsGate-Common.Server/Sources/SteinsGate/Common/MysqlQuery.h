/*
	생성일 : 2019/03/06
	수정일 : 2022/04/03 - 대거 수정함
	
	이게 옛날에 깃허브에서 검색 도중 발견했던건데 대거 수정함
	오류도 좀 있었고 다 뜯어고침
 */


#pragma once



#include <JCore/Assert.h>
#include <JCore/Time.h>
#include <JCore/Container/HashMap.h>

#include <SteinsGate/Common/MysqlConnection.h>
#include <SteinsGate/Common/MysqlStatementBuilder.h>
#include <SteinsGate/Common/StatementType.h>


class MysqlQuery;
class MysqlQuerySelect;
class MysqlQueryUpdate;
class MysqlQueryInsert;

using MysqlQueryPtr = JCore::SharedPtr<MysqlQuery>;
class MysqlQuery
{
public:
	static constexpr int StatementPrefixLength = JCore::StringUtil::CTLength("select");

	MysqlQuery(MysqlConnection* conn, const JCore::String& preparedStatement, StatementType type);
	virtual ~MysqlQuery() = default;

	// 앞 6글자를 확인해서 어떤 타입의 Statement인지 확인
	// 문자열이 select로 시작하면 SELECT 타입 - 뭐 이런식
	static StatementType ParseStatement(const JCore::String& statement);

	// 만들면서 동시에 인자를 넣어주도록...
	template <typename... Args>
	static MysqlQueryPtr Create(MysqlConnection* conn, const JCore::String& statement, Args&&... args) {
		const JCore::String preparedStatement = MysqlStatementBuilder::Build(statement, JCore::Forward<Args>(args)...);

		if (preparedStatement == "")
			return nullptr;

		MysqlQueryPtr spQuery;

		switch (ParseStatement(preparedStatement)) {
		case StatementType::Select: 
			spQuery = JCore::MakeShared<MysqlQuerySelect>(conn, preparedStatement, StatementType::Select);
			break;
		case StatementType::Update:
			spQuery = JCore::MakeShared<MysqlQueryUpdate>(conn, preparedStatement, StatementType::Update);
			break;
		case StatementType::Delete:
			spQuery = JCore::MakeShared<MysqlQueryUpdate>(conn, preparedStatement, StatementType::Delete);
			break;
		case StatementType::Insert: 
			spQuery = JCore::MakeShared<MysqlQueryInsert>(conn, preparedStatement, StatementType::Insert);
			break;
		default:
			DebugAssertMsg(false, "올바르지 않은 스테이트먼트입니다.");
		}


		return spQuery;
	}

	MysqlConnection* GetConnection() const { return m_pConn; }
	virtual bool Execute() = 0;
	virtual bool IsSuccess() const { return m_bSuccess; }
	virtual bool IsFailed() const { return !m_bSuccess; }
protected:
	StatementType m_eType;
	MysqlConnection* m_pConn;
	JCore::String m_PreparedStatement;
	bool m_bSuccess;
};

class MysqlQueryUpdate : public MysqlQuery
{
public:
	MysqlQueryUpdate(MysqlConnection* conn, const JCore::String& preparedStatement, StatementType type)
		: MysqlQuery(conn, preparedStatement, type)
	{}
	~MysqlQueryUpdate() override = default;

	bool Execute() override;
};

class MysqlQueryInsert : public MysqlQuery
{
public:
	MysqlQueryInsert(MysqlConnection* conn, const JCore::String& preparedStatement, StatementType type)
		: MysqlQuery(conn, preparedStatement, type)
		, m_uiInsertId(0)
	{}
	~MysqlQueryInsert() override = default;

	bool Execute() override;
	Int64U GetInsertId() const { return m_uiInsertId; }
private:
	Int64U m_uiInsertId;
};

class MysqlQuerySelect : public MysqlQuery
{
	using TResultRow = JCore::HashMap<int, JCore::String>;
public:
	MysqlQuerySelect(MysqlConnection* conn, const JCore::String& preparedStatement, StatementType type)
		: MysqlQuery(conn, preparedStatement, type)
	{}
	~MysqlQuerySelect() override = default;

	bool Execute() override;

	JCore::String GetFieldName(const unsigned int& field);
	JCore::String GetString(const unsigned int& rowIdx, const unsigned int& fieldIdx);
	JCore::String GetString(const unsigned int& rowIdx, const JCore::String& fieldName);
	int GetInt(const unsigned int& rowIdx, const unsigned int& fieldIdx);
	int GetInt(const unsigned int& rowIdx, const JCore::String& fieldName);
	double GetDouble(const unsigned int& rowIdx, const unsigned int& fieldIdx);
	double GetDouble(const unsigned int& rowIdx, const JCore::String& fieldName);
	JCore::DateTime GetTime(const unsigned int& rowIdx, const unsigned int& fieldIdx);
	JCore::DateTime GetTime(const unsigned int& rowIdx, const JCore::String& fieldName);

	unsigned int GetResultRowCount() const;
	unsigned int GetFieldCount() const;
private:
	JCore::HashMap<int, TResultRow> m_hResultMap;
	JCore::HashMap<int, JCore::String> m_hFieldMap;
	JCore::HashMap<JCore::String, int> m_hFieldStringToIntMap;
};

