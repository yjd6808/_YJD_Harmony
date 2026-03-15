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
	: colCount_(0)
	, hasCurrentRow_(false)
	{
	}

	virtual	~SqlServerQuerySelect() override;
	virtual	bool		 Execute() override;
	virtual	bool         HasNext() const override;
	virtual	bool         Next() override;
	virtual	int          GetColIndex(const char* _pFieldName) override;
	virtual	_u32         GetColCount() const override;
	virtual int			 GetColType(int _colIdx) const;

	virtual	const char*  GetRawString(const char* _pFieldName) override;
	virtual	const char*  GetRawString(int _fieldIndex) override;
	virtual	jc::DateTime GetDateTime(const char* _pFieldName) override;
	virtual	jc::DateTime GetDateTime(int _fieldIndex) override;

	virtual jc::String GetString(int _colIdx) override;
	virtual _s8  GetS8(int _colIdx) override;
	virtual _u8  GetU8(int _colIdx) override;
	virtual _s16 GetS16(int _colIdx) override;
	virtual _u16 GetU16(int _colIdx) override;
	virtual _s32 GetS32(int _colIdx) override;
	virtual _u32 GetU32(int _colIdx) override;
	virtual _s64 GetS64(int _colIdx) override;
	virtual _u64 GetU64(int _colIdx) override;
	virtual _f32 GetFloat(int _colIdx) override;
	virtual _f64 GetDouble(int _colIdx) override;
	virtual jc::Date GetDate(int _colIdx) override;
	virtual jc::Time GetTime(int _colIdx) override;

	virtual void		   ResetColReadOffset() override { colReadOffset_ = 0; }
	virtual jc::StringView ReadRawString() override;
	virtual jc::String	   ReadString() override;
	virtual _s8			   ReaS8() override;
	virtual _u8			   ReadU8() override;
	virtual _s16		   ReadS16() override;
	virtual _u16		   ReadU16() override;
	virtual _s32		   ReadS32() override;
	virtual _u32		   ReadU32() override;
	virtual _s64		   ReadS64() override;
	virtual _u64		   ReadU64() override;
	virtual _f32		   ReadFloat() override;
	virtual _f64		   ReadDouble() override;
	virtual jc::DateTime   ReadDateTime() override;

private:
	jc::DateTime ParseStringToDateTime(const char* _pRawString);

	static SQLULEN CalculateOptimalBufferSize_SQL_C_CHAR(SQLSMALLINT _sqlType, SQLULEN _columnSize);
private:
	static constexpr int COL_BUFFER_SIZE = 1024;

	struct ColumnBinder
	{
		char* pBuf_ = nullptr;
		SQLLEN length_ = 0;
		SQLLEN capacity_ = 0;
	};

	struct ColumnInfo
	{
		SQLSMALLINT type_ = 0;		// 컬럼 타입
		jc::String  name_;			// 컬럼 이름
	};

	jc::HashMap<jc::String, int> colNameMap_;		// 실제 개발 환경에서 쓸모 없긴한데.. 디버깅에도 용이하고 테스트에 용이하므로 일단 넣어놓음.
	jc::Vector<ColumnBinder>	 row_;
	jc::Vector<ColumnInfo>		 colInfoList_;
	SQLSMALLINT                  colCount_;
	_u32						 colReadOffset_ = 0;
	bool                         hasCurrentRow_;
};

NS_END
