#pragma once

#include <mysql.h>
#include <string>

class MySQLConnection
{

public:
	MySQLConnection();
	~MySQLConnection();

	// MySQL 접속
	bool Connect(const std::string &sHostname, const uint16_t &wPort, const std::string &sUsername, const std::string &sPassword, const std::string &sDB);

	// DB 선택
	bool SelectDB(const std::string &sSchemaName);

	// 연결 끊기
	void Disconnect();

	// 제일 마지막 오류
	const std::string GetLastError() const;

	MYSQL *getConn();
	bool IsConnected();
	// returns an escaped string
	const std::string EscapeString(const std::string &value) const;

private:
	MYSQL *m_MySQLConn;
	bool m_bIsConnected;
	std::string m_sHostname = "";
	std::string m_sUsername = "";
	std::string m_sPassword = "";
	std::string m_sSchemaName = "";
	uint16_t m_wPort;
};

