#include <TF/Database/Mysql/MysqlConnection.h>
#include <TF/Util/Console.h>

MySQLConnection::MySQLConnection()
	: m_MySQLConn(NULL)
	, m_bIsConnected(false)
	, m_wPort(0) {
}

MySQLConnection::~MySQLConnection()
{
	if (m_MySQLConn != NULL)
	{
		//std::clog << "Closing MySQL Connection" << std::endl;
		mysql_close(m_MySQLConn);
	}
}

bool MySQLConnection::Connect(const std::string &sHostname, const uint16_t &wPort, const std::string &sUsername, const std::string &sPassword, const std::string &sDB = NULL)
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

	MySQLConnRet = mysql_real_connect(m_MySQLConn, m_sHostname.c_str(), m_sUsername.c_str(), m_sPassword.c_str(), m_sSchemaName.c_str(), m_wPort, NULL, 0);

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

void MySQLConnection::Disconnect()
{
	if (m_bIsConnected)
	{
		mysql_close(m_MySQLConn);
		//std::clog << "Disconnected from MySQL DB!" << std::endl;
	}


	m_bIsConnected = false;

}

bool MySQLConnection::SelectDB(const std::string &sSchemaName)
{
	if (!m_bIsConnected)
	{
		Console::WriteLine(ConsoleColor::LIGHTRED, "[Error] Not connected to MySQL DB!");
		return false;
	}

	if (mysql_select_db(m_MySQLConn, sSchemaName.c_str()) != 0)
	{
		Console::WriteLine(ConsoleColor::LIGHTRED, "[Error] Failed to select DB! Error: %s", mysql_error(m_MySQLConn));
		return false;
	}
	else {
		m_sSchemaName = sSchemaName.c_str();
		Console::WriteLine(ConsoleColor::GREEN, "[Info] Selected database \"%s\"", sSchemaName.c_str());
		return true;
	}
}

const std::string MySQLConnection::GetLastError() const
{
	if (!m_bIsConnected)
	{
		Console::WriteLine(ConsoleColor::LIGHTRED, "[Error] Not connected to MySQL DB!");
		return "Not connected";
	}

	return (char*)mysql_error(m_MySQLConn);
}

MYSQL *MySQLConnection::getConn()
{
	return m_MySQLConn;
}

bool MySQLConnection::IsConnected()
{
	return m_bIsConnected;
}

const std::string MySQLConnection::EscapeString(const std::string &value) const
{
	if (!m_bIsConnected)
	{
		Console::ErrorWriteLine("Not connected to MySQL DB!");
		return "";
	}

	char *cValue = new char[(value.length() * 2) + 1];
	mysql_real_escape_string(m_MySQLConn, cValue, value.c_str(), value.length());

	std::string sRet = cValue;
	delete[] cValue;

	return sRet;
}