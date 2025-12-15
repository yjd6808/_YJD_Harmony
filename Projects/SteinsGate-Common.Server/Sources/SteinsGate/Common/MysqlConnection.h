#pragma once

#include <mysql.h>
#include <JCore/Primitives/String.h>

class MysqlConnection
{
public:
	MysqlConnection();
	~MysqlConnection();

	// MySQL 접속
	bool Connect(const JCore::String& _hostname, const uint16_t& _port, const JCore::String& _username,
	             const JCore::String& _password, const JCore::String& _dbName = NULL);

	// DB 선택
	bool SelectDB(const JCore::String& _schemaName);

	// 연결 끊기
	void Disconnect();

	// 제일 마지막 오류
	JCore::String GetLastErrorString() const;
	int GetLastErrorCode() const;

	MYSQL* GetConnection() const;
	bool IsConnected() const;

	// 이스케이프 스트링 반환
	JCore::String EscapeString(const JCore::String& _value) const;

private:
	MYSQL* mySqlConn_;
	bool isConnected_;
	JCore::String hostname_ = "";
	JCore::String username_ = "";
	JCore::String password_ = "";
	JCore::String schemaName_ = "";
	uint16_t port_;
};
