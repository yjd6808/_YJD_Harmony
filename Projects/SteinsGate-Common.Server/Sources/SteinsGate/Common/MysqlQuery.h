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
class MysqlQueryDelete;
class MysqlQueryInsert;


using MysqlQueryPtr = JCore::SharedPtr<MysqlQuery>;
using MysqlQuerySelectPtr = JCore::SharedPtr<MysqlQuerySelect>;
using MysqlQueryUpdatePtr = JCore::SharedPtr<MysqlQueryUpdate>;
using MysqlQueryDeletePtr = JCore::SharedPtr<MysqlQueryDelete>;
using MysqlQueryInsertPtr = JCore::SharedPtr<MysqlQueryInsert>;

class MysqlQuery
{
public:
	static constexpr int StatementPrefixLength = JCore::StringUtil::CTLength("select");

	MysqlQuery(MysqlConnection* _pConn, const JCore::String& _preparedStatement, StatementType _type);
	virtual ~MysqlQuery() = default;

	// 앞 6글자를 확인해서 어떤 타입의 Statement인지 확인
	// 문자열이 select로 시작하면 SELECT 타입 - 뭐 이런식
	static StatementType ParseStatement(const JCore::String& _statement);

	// 만들면서 동시에 인자를 넣어주도록...
	template <typename... Args>
	static MysqlQueryPtr Create(MysqlConnection* _pConn, const JCore::String& _statement, Args&&... _args)
	{
		const JCore::String preparedStatement =
			MysqlStatementBuilder::Build(_statement, JCore::Forward<Args>(_args)...);

		if (preparedStatement == "")
			return nullptr;

		MysqlQueryPtr query;

		switch (ParseStatement(preparedStatement))
		{
		case StatementType::Select:
			query = JCore::MakeShared<MysqlQuerySelect>(_pConn, preparedStatement, StatementType::Select);
			break;
		case StatementType::Update:
			query = JCore::MakeShared<MysqlQueryUpdate>(_pConn, preparedStatement, StatementType::Update);
			break;
		case StatementType::Delete:
			query = JCore::MakeShared<MysqlQueryDelete>(_pConn, preparedStatement, StatementType::Delete);
			break;
		case StatementType::Insert:
			query = JCore::MakeShared<MysqlQueryInsert>(_pConn, preparedStatement, StatementType::Insert);
			break;
		default:
			DebugAssertMsg(false, "올바르지 않은 스테이트먼트입니다.");
		}


		return query;
	}

	MysqlConnection* GetConnection() const { return conn_; }
	virtual bool Execute() = 0;

	bool IsSuccess() const { return errorCode_ == 0; }
	bool IsFailed() const { return errorCode_ != 0; }
	int GetErrorCode() const { return errorCode_; }

	// Insert Update Delete는 영향받은 행 수를 반환한다.
	virtual Int32U GetRowCount() const;

	StatementType GetStatementType() { return statementType_; }

protected:
	StatementType statementType_;
	MysqlConnection* conn_;
	JCore::String preparedStatement_;
	int errorCode_;
};

class MysqlQueryUpdate : public MysqlQuery
{
public:
	MysqlQueryUpdate(MysqlConnection* _pConn, const JCore::String& _preparedStatement, StatementType _type)
	: MysqlQuery(_pConn, _preparedStatement, _type)
	{
	}

	~MysqlQueryUpdate() override = default;

	bool Execute() override;
};

class MysqlQueryDelete : public MysqlQuery
{
public:
	MysqlQueryDelete(MysqlConnection* _pConn, const JCore::String& _preparedStatement, StatementType _type)
	: MysqlQuery(_pConn, _preparedStatement, _type)
	{
	}

	~MysqlQueryDelete() override = default;

	bool Execute() override;
};

class MysqlQueryInsert : public MysqlQuery
{
public:
	MysqlQueryInsert(MysqlConnection* _pConn, const JCore::String& _preparedStatement, StatementType _type)
	: MysqlQuery(_pConn, _preparedStatement, _type)
	, insertId_(0)
	{
	}

	~MysqlQueryInsert() override = default;

	bool Execute() override;
	Int64U GetInsertId() const { return insertId_; }

private:
	Int64U insertId_;
};

// 기존의 모든 행 로딩 방식에서 Lazy Loading 방식으로 변경
// 전부터 자꾸 거슬렸다.
// 참고 소스: https://github.com/otland/forgottenserver 깃허브 (src/database.cpp) 코드 참고
class MysqlQuerySelect : public MysqlQuery
{
public:
	MysqlQuerySelect(MysqlConnection* _pConn, const JCore::String& _preparedStatement, StatementType _type)
	: MysqlQuery(_pConn, _preparedStatement, _type)
	, sqlResult_(nullptr)
	, sqlRow_(nullptr)
	{
	}

	~MysqlQuerySelect() override;

	bool Execute() override;
	bool HasNext() const;
	bool Next();

	int GetFieldIndex(const char* _pFieldName);
	const char* GetRawString(const char* _pFieldName);

	// 잘못된 필드를 주입하거나, 해당 행의 필드가 비어있는 경우(NULL)인경우 JCore::String(0)반환
	JCore::String GetString(const char* _pFieldName);
	JCore::DateTime GetDateTime(const char* _pFieldName);

	template <typename TInteger>
	bool TryGetNumber(const char* _pFieldName, TInteger& _val, TInteger _defaultValue = 0)
	{
		const char* pRawString = GetRawString(_pFieldName);

		if (pRawString == nullptr)
		{
			_val = _defaultValue;
			return false;
		}

		return JCore::StringUtil::ToNumber<TInteger>(pRawString);
	}


	template <typename TInteger>
	TInteger GetNumber(const char* _pFieldName)
	{
		const char* pRawString = GetRawString(_pFieldName);
		if (pRawString == nullptr) return 0;
		return JCore::StringUtil::ToNumber<TInteger>(pRawString);
	}

	Int32U GetRowCount() const override;
	Int32U GetFieldCount() const;

private:
	MYSQL_RES* sqlResult_;
	MYSQL_ROW sqlRow_;

	JCore::HashMap<JCore::String, int> fieldList_;
};
