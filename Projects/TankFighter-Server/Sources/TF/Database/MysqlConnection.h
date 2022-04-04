#pragma once

#include <mysql.h>
#include <string>
#include <JCore/String.h>

class MysqlConnection
{

public:
	MysqlConnection();
	~MysqlConnection();

	// MySQL 접속
	bool Connect(const JCore::String &sHostname, const uint16_t &wPort, const JCore::String &sUsername, const JCore::String &sPassword, const JCore::String &sDB);

	// DB 선택
	bool SelectDB(const JCore::String &sSchemaName);

	// 연결 끊기
	void Disconnect();

	// 제일 마지막 오류
	const JCore::String GetLastError() const;

	MYSQL *getConn();
	bool IsConnected();
	// returns an escaped string
	const JCore::String EscapeString(const JCore::String& value) const;

private:
	MYSQL *m_MySQLConn;
	bool m_bIsConnected;
	JCore::String m_sHostname = "";
	JCore::String m_sUsername = "";
	JCore::String m_sPassword = "";
	JCore::String m_sSchemaName = "";
	uint16_t m_wPort;
};

