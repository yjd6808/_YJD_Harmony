#pragma once

#include <jc/Primitives/String.h>

NS_JDB_BEGIN

class JDB_DLL MysqlConnection
{
public:
	MysqlConnection();
	~MysqlConnection();

	// MySQL 접속
	bool Connect(const jc::String& _hostname, const uint16_t& _port, const jc::String& _username,
	             const jc::String& _password, const jc::String& _dbName = NULL);

	// DB 선택
	bool SelectDB(const jc::String& _schemaName);

	// 연결 끊기
	void Disconnect();

	// 제일 마지막 오류
	jc::String GetLastErrorString() const;
	int GetLastErrorCode() const;

	MYSQL* GetConnection() const;
	bool IsConnected() const;

	// 이스케이프 스트링 반환
	jc::String EscapeString(const jc::String& _value) const;

private:
	MYSQL* mySqlConn_;
	bool isConnected_;
	jc::String hostname_ = "";
	jc::String username_ = "";
	jc::String password_ = "";
	jc::String schemaName_ = "";
	uint16_t port_;
};

NS_END