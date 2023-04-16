/*
	생성일 : 2019/03/06
	수정일 : 2022/04/03 - 대거 수정함

	원본 코드: https://github.com/LarkHunter/MySQLPool/tree/1770041934bf0308d00df097d5ed43a884b2f15e
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

// 기존의 모든 행 로딩 방식에서 Lazy Loading 방식으로 변경
// 전부터 자꾸 거슬렸다.
// 참고 소스: https://github.com/otland/forgottenserver 깃허브 (src/database.cpp) 코드 참고
class MysqlQuerySelect : public MysqlQuery
{
public:
	MysqlQuerySelect(MysqlConnection* conn, const JCore::String& preparedStatement, StatementType type)
		: MysqlQuery(conn, preparedStatement, type)
		, m_SqlResult(nullptr)
		, m_SqlRow(nullptr)
	{}
	~MysqlQuerySelect() override;

	bool Execute() override;
	bool HasNext() const;
	bool Next();

	int GetFieldIndex(const JCore::String& fieldName);
	char* GetRawString(const JCore::String& fieldName);

	// 잘못된 필드를 주입하거나, 해당 행의 필드가 비어있는 경우(NULL)인경우 JCore::String(0)반환
	JCore::String GetString(const JCore::String& fieldName);

	template <typename TInteger>
	bool TryGetNumber(const JCore::String& fieldName, TInteger& val, TInteger defaultValue = 0) {
		const char* pRawString = GetRawString(fieldName);

		if (pRawString == nullptr) {
			val = defaultValue;
			return false;
		}

		return JCore::StringUtil::ToNumber<TInteger>(pRawString);
	}

	template <typename TInteger>
	TInteger GetNumber(const JCore::String& fieldName) {
		const char* pRawString = GetRawString(fieldName);
		if (pRawString == nullptr) return 0;
		return JCore::StringUtil::ToNumber<TInteger>(pRawString);
	}

	Int32U GetRowCount() const;
	Int32U GetFieldCount() const;

private:
	MYSQL_RES*	m_SqlResult;
	MYSQL_ROW	m_SqlRow;

	JCore::HashMap<JCore::String, int> m_hFieldList;
};

