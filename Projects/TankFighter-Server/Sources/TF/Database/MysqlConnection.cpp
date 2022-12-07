#include <TF/PrecompiledHeader.h>
#include <TF/Database/MysqlConnection.h>

#include <JCore/Utils/Console.h>

using namespace JCore;

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
	// 이미 연결된 경우 우선 연결을 끊어준다.
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
		SafeConsole::WriteLine(ConsoleColor::LightGray, "MySQL 데이터베이스 연결 실패 : %s", mysql_error(m_MySQLConn));
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
		SafeConsole::WriteLine(ConsoleColor::LightGray, "SelectDB() 실패 : MySQL 데이터베이스에 연결되어 있지 않습니다.");
		return false;
	}

	if (mysql_select_db(m_MySQLConn, schemaName.Source()) != 0) {
		SafeConsole::WriteLine(ConsoleColor::LightGray, "SelectDB() 실패 : mysql_select_db() 호출 실패 : %s", mysql_error(m_MySQLConn));
		return false;
	}

	m_sSchemaName = schemaName.Source();
	SafeConsole::WriteLine(ConsoleColor::Green, "SelectDB() 성공 : \"%s\"", schemaName.Source());
	return true;
}

JCore::String MysqlConnection::GetLastError() const {
	if (!m_bIsConnected) {
		SafeConsole::WriteLine(ConsoleColor::LightGray, "GetLastError() 실패 : MySQL 데이터베이스에 연결되어 있지 않습니다.");
		return "연결 안되있음";
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
		SafeConsole::WriteLine("DB에 연결되어있지 않습니다.");
		return "";
	}

	char temp[1024];
	JCore::String escapedString(value.Length() * 2 + 1);

	mysql_real_escape_string(m_MySQLConn, temp, value.Source(), value.Length());

	// 여기서 걍 따옴표 달아주면 대네
	escapedString += "\"";
	escapedString += temp;
	escapedString += "\"";
	return escapedString;
}

