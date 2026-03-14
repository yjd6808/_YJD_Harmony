/*
	생성일 : 2019/03/06
	수정일 : 2022/04/03 - 대거 수정함
	수정일 : 2026/03/13 - IQuery 기반 계층 구조로 재설계

	원본 코드: https://github.com/LarkHunter/MySQLPool/tree/1770041934bf0308d00df097d5ed43a884b2f15e
	이게 옛날에 깃허브에서 검색 도중 발견했던건데 대거 수정함
	오류도 좀 있었고 다 뜯어고침
 */


#pragma once

#include <jc/Assert.h>
#include <jc/Time.h>
#include <jc/Container/HashMap.h>

#include "MysqlConnection.h"
#include "../IQuery.h"

NS_JDB_BEGIN

// ===========================================================================================
// MysqlQuery: MySQL 쿼리 구현 베이스
// ===========================================================================================

class JDB_DLL MysqlQuery : public IQuery
{
public:
	MysqlQuery() = default;
	virtual ~MysqlQuery() override = default;

protected:
	static MysqlConnection* AsMysqlConn(IConnection* _pConn)
	{
		return static_cast<MysqlConnection*>(_pConn);
	}
};

// ===========================================================================================
// MysqlQueryUpdate
// ===========================================================================================

class MysqlQueryUpdate : public MysqlQuery
{
public:
	virtual ~MysqlQueryUpdate() override = default;
	virtual bool Execute() override;
};

// ===========================================================================================
// MysqlQueryDelete
// ===========================================================================================

class MysqlQueryDelete : public MysqlQuery
{
public:
	virtual ~MysqlQueryDelete() override = default;
	virtual bool Execute() override;
};

// ===========================================================================================
// MysqlQueryInsert
// ===========================================================================================

class MysqlQueryInsert : public MysqlQuery
{
public:
	MysqlQueryInsert() : insertId_(0) {}
	~MysqlQueryInsert() override = default;

	virtual _u64 GetInsertId() const override { return insertId_; }
	virtual bool Execute() override;

private:
	_u64 insertId_;
};

// ===========================================================================================
// MysqlQuerySelect
// ===========================================================================================

class MysqlQuerySelect : public MysqlQuery
{
public:
	MysqlQuerySelect()
	: sqlResult_(nullptr)
	, sqlRow_(nullptr)
	{
	}

	~MysqlQuerySelect() override;

	virtual bool Execute() override;
	virtual bool         HasNext() const override;
	virtual bool         Next() override;
	virtual int          GetFieldIndex(const char* _pFieldName) override;
	virtual const char*  GetRawString(const char* _pFieldName) override;
	virtual const char*  GetRawString(int _fieldIndex) override;
	virtual jc::DateTime GetDateTime(const char* _pFieldName) override;
	virtual jc::DateTime GetDateTime(int _fieldIndex) override;
	virtual _u32         GetFieldCount() const override;

private:
	jc::DateTime ParseRawStringToDateTime(const char* _pRawString);

private:
	MYSQL_RES* sqlResult_;
	MYSQL_ROW  sqlRow_;

	jc::HashMap<jc::String, int> fieldList_;
};

NS_END