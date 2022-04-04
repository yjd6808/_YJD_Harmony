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
	{
		//std::clog << "Closing MySQL Connection" << std::endl;
		mysql_close(m_MySQLConn);
	}
}

bool MysqlConnection::Connect(const JCore::String &sHostname, const uint16_t &wPort, const JCore::String &sUsername, const JCore::String &sPassword, const JCore::String &sDB = NULL)
{
	// If we're already connected, we should close the first connection
	Disconnect();

	m_sHostname = sHostname;
	m_sUsername = sUsername;
	m_sPassword = sPassword;
	m_wPort = wPort;
	m_sSchemaName = sDB;
	m_bIsConnected = false;

	MYSQL *MySQLConnRet = NULL;
	m_MySQLConn = mysql_init(m_MySQLConn);

	mysql_options(m_MySQLConn, MYSQL_SET_CHARSET_NAME, "utf8");
	mysql_options(m_MySQLConn, MYSQL_INIT_COMMAND, "SET NAMES utf8");


	MySQLConnRet = mysql_real_connect(m_MySQLConn, m_sHostname.Source(), m_sUsername.Source(), m_sPassword.Source(), m_sSchemaName.Source(), m_wPort, NULL, 0);

	if (MySQLConnRet == NULL)
	{
		m_bIsConnected = false;
		Console::WriteLine(ConsoleColor::LIGHTRED, "[Error] Connection failed: %s", mysql_error(m_MySQLConn));
	}
	else {
		m_bIsConnected = true;
	}

	return m_bIsConnected;
}

void MysqlConnection::Disconnect()
{
	if (m_bIsConnected)
	{
		mysql_close(m_MySQLConn);
		//std::clog << "Disconnected from MySQL DB!" << std::endl;
	}


	m_bIsConnected = false;

}

bool MysqlConnection::SelectDB(const JCore::String &sSchemaName)
{
	if (!m_bIsConnected)
	{
		Console::WriteLine(ConsoleColor::LIGHTRED, "[Error] Not connected to MySQL DB!");
		return false;
	}

	if (mysql_select_db(m_MySQLConn, sSchemaName.Source()) != 0)
	{
		Console::WriteLine(ConsoleColor::LIGHTRED, "[Error] Failed to select DB! Error: %s", mysql_error(m_MySQLConn));
		return false;
	}
	else {
		m_sSchemaName = sSchemaName.Source();
		Console::WriteLine(ConsoleColor::GREEN, "[Info] Selected database \"%s\"", sSchemaName.Source());
		return true;
	}
}

const JCore::String MysqlConnection::GetLastError() const
{
	if (!m_bIsConnected)
	{
		Console::WriteLine(ConsoleColor::LIGHTRED, "[Error] Not connected to MySQL DB!");
		return "Not connected";
	}

	return (char*)mysql_error(m_MySQLConn);
}

MYSQL *MysqlConnection::getConn()
{
	return m_MySQLConn;
}

bool MysqlConnection::IsConnected()
{
	return m_bIsConnected;
}

const JCore::String MysqlConnection::EscapeString(const JCore::String& value) const
{
	if (!m_bIsConnected)
	{
		Console::WriteLine("DB에 연결되어있지 않습니다.");
		return "";
	}

	char *cValue = new char[(value.Length() * 2) + 1];
	mysql_real_escape_string(m_MySQLConn, cValue, value.Source(), value.Length());

	JCore::String sRet = cValue;
	delete[] cValue;

	return sRet;
}

