#pragma once

#include <mysql.h>
#include <JCore/Primitives/String.h>

class MysqlConnection
{

public:
	MysqlConnection();
	~MysqlConnection();

	// MySQL 접속
	bool Connect(const JCore::String &hostname, const uint16_t &port, const JCore::String &username, const JCore::String &password, const JCore::String &dbname);

	// DB 선택
	bool SelectDB(const JCore::String &schemaName);

	// 연결 끊기
	void Disconnect();

	// 제일 마지막 오류
	JCore::String GetLastError() const;

	MYSQL *GetConnection() const;
	bool IsConnected() const;

	// 이스케이프 스트링 반환
	JCore::String EscapeString(const JCore::String& value) const;

private:
	MYSQL *m_MySQLConn;
	bool m_bIsConnected;
	JCore::String m_sHostname = "";
	JCore::String m_sUsername = "";
	JCore::String m_sPassword = "";
	JCore::String m_sSchemaName = "";
	uint16_t m_wPort;
};

