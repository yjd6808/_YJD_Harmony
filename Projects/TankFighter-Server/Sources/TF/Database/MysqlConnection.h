#pragma once

#include <mysql.h>
#include <JCore/String.h>

class MysqlConnection
{

public:
	MysqlConnection();
	~MysqlConnection();

	// MySQL ����
	bool Connect(const JCore::String &hostname, const uint16_t &port, const JCore::String &username, const JCore::String &password, const JCore::String &dbname);

	// DB ����
	bool SelectDB(const JCore::String &schemaName);

	// ���� ����
	void Disconnect();

	// ���� ������ ����
	JCore::String GetLastError() const;

	MYSQL *GetConnection() const;
	bool IsConnected() const;

	// �̽������� ��Ʈ�� ��ȯ
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

