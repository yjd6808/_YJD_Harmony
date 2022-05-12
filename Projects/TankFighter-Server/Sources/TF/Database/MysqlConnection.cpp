#include <TF/PrecompiledHeader.h>
#include <TF/Database/MysqlConnection.h>
#include <TF/Util/Console.h>


MysqlConnection::MysqlConnection()
	: m_MySQLConn(NULL)
	, m_bIsConnected(false)
	, m_wPort(0) {
}

MysqlConnection::~MysqlConnection()
{
	if (m_MySQLConn != NULL)
		mysql_close(m_MySQLConn);
}

bool MysqlConnection::Connect(const JCore::String &hostname, const uint16_t &port, const JCore::String &username, const JCore::String &password, const JCore::String &dbname = NULL)
{
	// �̹� ����� ��� �켱 ������ �����ش�.
	Disconnect();

	m_sHostname = hostname;
	m_sUsername = username;
	m_sPassword = password;
	m_wPort = port;
	m_sSchemaName = dbname;
	m_bIsConnected = false;

	MYSQL *MySQLConnRet = NULL;
	m_MySQLConn = mysql_init(m_MySQLConn);

	mysql_options(m_MySQLConn, MYSQL_SET_CHARSET_NAME, "utf8");
	mysql_options(m_MySQLConn, MYSQL_INIT_COMMAND, "SET NAMES utf8");


	MySQLConnRet = mysql_real_connect(m_MySQLConn, m_sHostname.Source(), m_sUsername.Source(), m_sPassword.Source(), m_sSchemaName.Source(), m_wPort, NULL, 0);

	if (MySQLConnRet == NULL) {
		m_bIsConnected = false;
		Console::WriteLine(ConsoleColor::LIGHTRED, "MySQL �����ͺ��̽� ���� ���� : %s", mysql_error(m_MySQLConn));
	} else {
		m_bIsConnected = true;
	}

	return m_bIsConnected;
}

void MysqlConnection::Disconnect() {
	if (m_bIsConnected) {
		mysql_close(m_MySQLConn);
		m_MySQLConn = nullptr;
	}

	m_bIsConnected = false;

}

bool MysqlConnection::SelectDB(const JCore::String &schemaName)
{
	if (!m_bIsConnected) {
		Console::WriteLine(ConsoleColor::LIGHTRED, "SelectDB() ���� : MySQL �����ͺ��̽��� ����Ǿ� ���� �ʽ��ϴ�.");
		return false;
	}

	if (mysql_select_db(m_MySQLConn, schemaName.Source()) != 0) {
		Console::WriteLine(ConsoleColor::LIGHTRED, "SelectDB() ���� : mysql_select_db() ȣ�� ���� : %s", mysql_error(m_MySQLConn));
		return false;
	}

	m_sSchemaName = schemaName.Source();
	Console::WriteLine(ConsoleColor::GREEN, "SelectDB() ���� : \"%s\"", schemaName.Source());
	return true;
}

JCore::String MysqlConnection::GetLastError() const {
	if (!m_bIsConnected) {
		Console::WriteLine(ConsoleColor::LIGHTRED, "GetLastError() ���� : MySQL �����ͺ��̽��� ����Ǿ� ���� �ʽ��ϴ�.");
		return "���� �ȵ�����";
	}

	return (char*)mysql_error(m_MySQLConn);
}

MYSQL *MysqlConnection::GetConnection() const {
	return m_MySQLConn;
}

bool MysqlConnection::IsConnected() const {
	return m_bIsConnected;
}

JCore::String MysqlConnection::EscapeString(const JCore::String& value) const {
	if (!m_bIsConnected) {
		Console::WriteLine("DB�� ����Ǿ����� �ʽ��ϴ�.");
		return "";
	}

	char temp[1024];
	JCore::String escapedString(value.Length() * 2 + 1);

	mysql_real_escape_string(m_MySQLConn, temp, value.Source(), value.Length());

	// ���⼭ �� ����ǥ �޾��ָ� ���
	escapedString += "\"";
	escapedString += temp;
	escapedString += "\"";
	return escapedString;
}

