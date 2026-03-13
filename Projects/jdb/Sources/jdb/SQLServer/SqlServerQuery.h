/*
	생성일 : 2026/03/07
	작성자 : 윤정도

	SQL Server ODBC 기반 쿼리 클래스
	MySQL 쿼리 클래스와 동일한 인터페이스를 제공한다.
 */


#pragma once

#include <jc/Assert.h>
#include <jc/Time.h>
#include <jc/Container/HashMap.h>
#include <jc/Container/Vector.h>

#include "SqlServerConnection.h"
#include "SqlServerStatementBuilder.h"
#include "../StatementType.h"

NS_JDB_BEGIN

class SqlServerQuery;
class SqlServerQuerySelect;
class SqlServerQueryUpdate;
class SqlServerQueryDelete;
class SqlServerQueryInsert;

using SqlServerQueryPtr = jc::SharedPtr<SqlServerQuery>;
using SqlServerQuerySelectPtr = jc::SharedPtr<SqlServerQuerySelect>;
using SqlServerQueryUpdatePtr = jc::SharedPtr<SqlServerQueryUpdate>;
using SqlServerQueryDeletePtr = jc::SharedPtr<SqlServerQueryDelete>;
using SqlServerQueryInsertPtr = jc::SharedPtr<SqlServerQueryInsert>;

class JDB_DLL SqlServerQuery
{
public:
	static constexpr int StatementPrefixLength = jc::StringUtil::CTLength("select");

	SqlServerQuery(SqlServerConnection* _pConn, const jc::String& _preparedStatement, StatementType _type);
	virtual ~SqlServerQuery();

	static StatementType ParseStatement(const jc::String& _statement);

	template <typename... Args>
	static SqlServerQueryPtr Create(SqlServerConnection* _pConn, const jc::String& _statement, Args&&... _args)
	{
		const jc::String preparedStatement =
			SqlServerStatementBuilder::Build(_statement, jc::Forward<Args>(_args)...);

		if (preparedStatement == "")
			return nullptr;

		SqlServerQueryPtr query;

		switch (ParseStatement(preparedStatement))
		{
		case StatementType::Select:
			query = jc::MakeShared<SqlServerQuerySelect>(_pConn, preparedStatement, StatementType::Select);
			break;
		case StatementType::Update:
			query = jc::MakeShared<SqlServerQueryUpdate>(_pConn, preparedStatement, StatementType::Update);
			break;
		case StatementType::Delete:
			query = jc::MakeShared<SqlServerQueryDelete>(_pConn, preparedStatement, StatementType::Delete);
			break;
		case StatementType::Insert:
			query = jc::MakeShared<SqlServerQueryInsert>(_pConn, preparedStatement, StatementType::Insert);
			break;
		default:
			jc_assert_msg(false, "올바르지 않은 스테이트먼트입니다.");
		}

		return query;
	}

	SqlServerConnection* GetConnection() const { return conn_; }
	virtual bool Execute() = 0;

	bool IsSuccess() const { return errorCode_ == 0; }
	bool IsFailed() const { return errorCode_ != 0; }
	int GetErrorCode() const { return errorCode_; }

	virtual _u32 GetRowCount() const;

	StatementType GetStatementType() { return statementType_; }

protected:
	void ExtractError(SQLHSTMT _hStmt);
	void FreeStatement();

protected:
	StatementType statementType_;
	SqlServerConnection* conn_;
	jc::String preparedStatement_;
	int errorCode_;
	SQLHSTMT hStmt_;
};

class SqlServerQueryUpdate : public SqlServerQuery
{
public:
	SqlServerQueryUpdate(SqlServerConnection* _pConn, const jc::String& _preparedStatement, StatementType _type)
	: SqlServerQuery(_pConn, _preparedStatement, _type)
	{
	}

	~SqlServerQueryUpdate() override = default;

	bool Execute() override;
};

class SqlServerQueryDelete : public SqlServerQuery
{
public:
	SqlServerQueryDelete(SqlServerConnection* _pConn, const jc::String& _preparedStatement, StatementType _type)
	: SqlServerQuery(_pConn, _preparedStatement, _type)
	{
	}

	~SqlServerQueryDelete() override = default;

	bool Execute() override;
};

class SqlServerQueryInsert : public SqlServerQuery
{
public:
	SqlServerQueryInsert(SqlServerConnection* _pConn, const jc::String& _preparedStatement, StatementType _type)
	: SqlServerQuery(_pConn, _preparedStatement, _type)
	, insertId_(0)
	{
	}

	~SqlServerQueryInsert() override = default;

	bool Execute() override;
	_u64 GetInsertId() const { return insertId_; }

private:
	_u64 insertId_;
};

class SqlServerQuerySelect : public SqlServerQuery
{
public:
	SqlServerQuerySelect(SqlServerConnection* _pConn, const jc::String& _preparedStatement, StatementType _type)
	: SqlServerQuery(_pConn, _preparedStatement, _type)
	, rowCount_(0)
	, columnCount_(0)
	, hasCurrentRow_(false)
	{
	}

	~SqlServerQuerySelect() override = default;

	bool Execute() override;
	bool HasNext() const;
	bool Next();

	int GetFieldIndex(const char* _pFieldName);

	const char* GetRawString(const char* _pFieldName);
	const char* GetRawString(int _fieldIndex);

	jc::String GetString(const char* _pFieldName);
	jc::String GetString(int _fieldIndex);

	jc::DateTime GetDateTime(const char* _pFieldName);
	jc::DateTime GetDateTime(int _fieldIndex);

	_u32 GetU32(const char* _pFieldName);
	_u32 GetU32(int _fieldIndex);
	_s32 GetS32(const char* _pFieldName);
	_s32 GetS32(int _fieldIndex);
	_u16 GetU16(const char* _pFieldName);
	_u16 GetU16(int _fieldIndex);
	_s16 GetS16(const char* _pFieldName);
	_s16 GetS16(int _fieldIndex);
	_u8 GetU8(const char* _pFieldName);
	_u8 GetU8(int _fieldIndex);
	_s8 GetS8(const char* _pFieldName);
	_s8 GetS8(int _fieldIndex);
	_u64 GetU64(const char* _pFieldName);
	_u64 GetU64(int _fieldIndex);
	_s64 GetS64(const char* _pFieldName);
	_s64 GetS64(int _fieldIndex);

	template <typename TInteger>
	bool TryGetNumber(const char* _pFieldName, TInteger& _val, TInteger _defaultValue = 0)
	{
		const char* pRawString = GetRawString(_pFieldName);

		if (pRawString == nullptr)
		{
			_val = _defaultValue;
			return false;
		}

		return jc::StringUtil::ToNumber<TInteger>(pRawString);
	}

	template <typename TInteger>
	bool TryGetNumber(int _fieldIndex, TInteger& _val, TInteger _defaultValue = 0)
	{
		const char* pRawString = GetRawString(_fieldIndex);

		if (pRawString == nullptr)
		{
			_val = _defaultValue;
			return false;
		}

		return jc::StringUtil::ToNumber<TInteger>(pRawString);
	}

	template <typename TInteger>
	TInteger GetNumber(const char* _pFieldName)
	{
		const char* pRawString = GetRawString(_pFieldName);
		if (pRawString == nullptr) return 0;
		return jc::StringUtil::ToNumber<TInteger>(pRawString);
	}

	template <typename TInteger>
	TInteger GetNumber(int _fieldIndex)
	{
		const char* pRawString = GetRawString(_fieldIndex);
		if (pRawString == nullptr) return 0;
		return jc::StringUtil::ToNumber<TInteger>(pRawString);
	}

	_u32 GetRowCount() const override;
	_u32 GetFieldCount() const;

private:
	void LoadCurrentRowData();
	jc::DateTime ParseStringToDateTime(const char* _pRawString);

private:
	jc::HashMap<jc::String, int> fieldList_;
	jc::Vector<jc::String> rowData_;
	_u32 rowCount_;
	SQLSMALLINT columnCount_;
	bool hasCurrentRow_;
};

NS_END
