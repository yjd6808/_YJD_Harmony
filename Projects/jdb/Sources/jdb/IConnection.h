/*
 * 작성자: 윤정도
 * 생성일: 3/13/2026
 * =====================
 *
 * MySQL/SQLServer 공통 커넥션 추상 인터페이스
 */

#pragma once

#include <jc/Primitives/String.h>

#include <jdb/Structure.h>
#include <jdb/DLLExport.h>

NS_JDB_BEGIN

class JDB_DLL IConnection
{
public:
	virtual ~IConnection() = default;

	virtual bool Connect(const jc::String& _hostname, const uint16_t& _port, const jc::String& _username,
	                     const jc::String& _password, const jc::String& _dbName = "") = 0;
	virtual bool SelectDB(const jc::String& _dbName) = 0;
	virtual void Disconnect() = 0;

	virtual jc::String GetLastErrorString() const = 0;
	virtual int GetLastErrorCode() const = 0;
	virtual int GetLastError(OUT jc::String& _str) const = 0;

	virtual jc::String EscapeString(const jc::String& _value) const = 0;

	DatabaseType GetDBType() const { return dbType_; }
	bool IsConnected() const { return isConnected_; }

protected:
	DatabaseType dbType_ = DatabaseType::None;

	jc::String hostname_;
	jc::String username_;
	jc::String password_;
	jc::String dbName_;
	uint16_t port_ = 0;
	bool isConnected_ = false;
};

NS_END
