#include "Core.h"
#include "MysqlConnection.h"

#include <jc/Utils/Console.h>

USING_NS_JC;
USING_NS_STD;

//////////////////////////////////////////////////////////////////////////////////////////
MysqlConnection::MysqlConnection()
: mySqlConn_(nullptr)
, isConnected_(false)
, port_(0)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
MysqlConnection::~MysqlConnection()
{
	if (mySqlConn_ != nullptr)
	{
		mysql_close(mySqlConn_);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
bool MysqlConnection::Connect(const jc::String& _hostname, const uint16_t& _port, const jc::String& _username,
                              const jc::String& _password, const jc::String& _dbName)
{
	Disconnect();

	hostname_ = _hostname;
	username_ = _username;
	password_ = _password;
	port_ = _port;
	schemaName_ = _dbName;
	isConnected_ = false;

	MYSQL* pMySqlConnRet = nullptr;
	mySqlConn_ = mysql_init(mySqlConn_);

	mysql_options(mySqlConn_, MYSQL_SET_CHARSET_NAME, "utf8");
	mysql_options(mySqlConn_, MYSQL_INIT_COMMAND, "SET NAMES utf8");

	pMySqlConnRet = mysql_real_connect(mySqlConn_, hostname_.Source(), username_.Source(), password_.Source(),
	                                   schemaName_.Source(), port_, NULL, 0);

	if (pMySqlConnRet == nullptr)
	{
		isConnected_ = false;
		_LogError_("MySQL 데이터베이스 연결 실패 : %s", mysql_error(mySqlConn_));
	}
	else
	{
		isConnected_ = true;
	}

	return isConnected_;
}

//////////////////////////////////////////////////////////////////////////////////////////
void MysqlConnection::Disconnect()
{
	if (isConnected_)
	{
		mysql_close(mySqlConn_);
		mySqlConn_ = nullptr;
	}

	isConnected_ = false;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool MysqlConnection::SelectDB(const jc::String& _schemaName)
{
	if (!isConnected_)
	{
		_LogError_("SelectDB() 실패 : MySQL 데이터베이스에 연결되어 있지 않습니다.");
		return false;
	}

	if (mysql_select_db(mySqlConn_, _schemaName.Source()) != 0)
	{
		_LogError_("SelectDB() 실패 : mysql_select_db() 호출 실패 : %s", mysql_error(mySqlConn_));
		return false;
	}

	schemaName_ = _schemaName.Source();
	_LogDebug_("SelectDB() 성공 : \"%s\"", _schemaName.Source());
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
jc::String MysqlConnection::GetLastErrorString() const
{
	if (!isConnected_)
	{
		_LogWarn_("GetLastErrorString() 실패 : MySQL 데이터베이스에 연결되어 있지 않습니다.");
		return "연결 안되있음";
	}

	return (char*)mysql_error(mySqlConn_);
}

//////////////////////////////////////////////////////////////////////////////////////////
int MysqlConnection::GetLastErrorCode() const
{
	if (!isConnected_)
	{
		_LogWarn_("GetLastErrorCode() 실패 : MySQL 데이터베이스에 연결되어 있지 않습니다.");
		return -1;
	}

	return static_cast<int>(mysql_errno(mySqlConn_));
}

//////////////////////////////////////////////////////////////////////////////////////////
MYSQL* MysqlConnection::GetConnection() const
{
	return mySqlConn_;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool MysqlConnection::IsConnected() const
{
	return isConnected_;
}

//////////////////////////////////////////////////////////////////////////////////////////
jc::String MysqlConnection::EscapeString(const jc::String& _value) const
{
	if (!isConnected_)
	{
		_LogWarn_("DB에 연결되어있지 않습니다.");
		return "";
	}

	char temp[1024];
	jc::String escapedString(_value.Length() * 2 + 1);

	mysql_real_escape_string(mySqlConn_, temp, _value.Source(), _value.Length());

	escapedString += "\"";
	escapedString += temp;
	escapedString += "\"";
	return escapedString;
}
