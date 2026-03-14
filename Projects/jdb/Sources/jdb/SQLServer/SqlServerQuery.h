/*
	생성일 : 2026/03/07
	작성자 : 윤정도
	수정일 : 2026/03/13 - IQuery 기반 계층 구조로 재설계

	SQL Server ODBC 기반 쿼리 클래스
 */

#pragma once

#include <jc/Assert.h>
#include <jc/Time.h>
#include <jc/Container/HashMap.h>
#include <jc/Container/Vector.h>

#include "SqlServerConnection.h"
#include "../IQuery.h"

NS_JDB_BEGIN

// ===========================================================================================
// SqlServerQuery: SQL Server 쿼리 구현 베이스
// ===========================================================================================

class JDB_DLL SqlServerQuery : public IQuery
{
public:
	SqlServerQuery() : hStmt_(SQL_NULL_HSTMT) {}
	virtual ~SqlServerQuery() override;

protected:
	static SqlServerConnection* AsSqlServerConn(IConnection* _pConn)
	{
		return static_cast<SqlServerConnection*>(_pConn);
	}

	void ExtractError(SQLHSTMT _hStmt);
	void FreeStatement();

protected:
	SQLHSTMT hStmt_;
};

// ===========================================================================================
// SqlServerQueryUpdate
// ===========================================================================================

class SqlServerQueryUpdate : public SqlServerQuery
{
public:
	virtual~SqlServerQueryUpdate() override = default;
	virtual bool Execute() override;
};

// ===========================================================================================
// SqlServerQueryDelete
// ===========================================================================================

class SqlServerQueryDelete : public SqlServerQuery
{
public:
	~SqlServerQueryDelete() override = default;

	virtual bool Execute() override;
};

// ===========================================================================================
// SqlServerQueryInsert
// ===========================================================================================

class SqlServerQueryInsert : public SqlServerQuery
{
public:
	SqlServerQueryInsert() : insertId_(0) {}
	virtual ~SqlServerQueryInsert() override = default;
	virtual _u64 GetInsertId() const override { return insertId_; }
	virtual bool Execute() override;

private:
	_u64 insertId_;
};

// ===========================================================================================
// SqlServerQuerySelect
// ===========================================================================================

class SqlServerQuerySelect : public SqlServerQuery
{
public:
	SqlServerQuerySelect()
	: columnCount_(0)
	, hasCurrentRow_(false)
	{
	}

	virtual	~SqlServerQuerySelect() override;
	virtual	virtual bool Execute() override;
	virtual	bool         HasNext() const override;
	virtual	bool         Next() override;
	virtual	int          GetFieldIndex(const char* _pFieldName) override;
	virtual	const char*  GetRawString(const char* _pFieldName) override;
	virtual	const char*  GetRawString(int _fieldIndex) override;
	virtual	jc::DateTime GetDateTime(const char* _pFieldName) override;
	virtual	jc::DateTime GetDateTime(int _fieldIndex) override;
	virtual	_u32         GetFieldCount() const override;

private:
	jc::DateTime ParseStringToDateTime(const char* _pRawString);

	static SQLULEN CalculateOptimalBufferSize(SQLSMALLINT _sqlType, SQLULEN _columnSize);
private:
	static constexpr int COL_BUFFER_SIZE = 1024;

	struct ColumnBinder
	{
		char* pBuf_ = nullptr;
		SQLLEN length_ = 0;
		SQLLEN capacity_ = 0;
	};

	jc::HashMap<jc::String, int> fieldList_;
	jc::Vector<ColumnBinder>	 row_;

	SQLSMALLINT                  columnCount_;
	bool                         hasCurrentRow_;
};

NS_END
