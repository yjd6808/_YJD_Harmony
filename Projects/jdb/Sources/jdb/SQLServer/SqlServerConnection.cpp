#include "Core.h"
#include "SqlServerConnection.h"

#include <jc/Utils/Console.h>

USING_NS_JC;
USING_NS_STD;

NS_JDB_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
SqlServerConnection::SqlServerConnection()
: hEnv_(SQL_NULL_HENV)
, hDbc_(SQL_NULL_HDBC)
{
	dbType_ = DatabaseType::dbtSQLServer;
}

//////////////////////////////////////////////////////////////////////////////////////////
SqlServerConnection::~SqlServerConnection()
{
	SqlServerConnection::Disconnect();
}

//////////////////////////////////////////////////////////////////////////////////////////
bool SqlServerConnection::Connect(const jc::String& _hostname, const uint16_t& _port, const jc::String& _username,
                                  const jc::String& _password, const jc::String& _dbName)
{
	Disconnect();

	hostname_ = _hostname;
	username_ = _username;
	password_ = _password;
	port_ = _port;
	dbName_ = _dbName;
	isConnected_ = false;

	SQLRETURN ret;

	// ODBC 환경 핸들 할당
	ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv_);
	if (!SQL_SUCCEEDED(ret))
	{
		_LogError_("SQLServer ODBC 환경 핸들 할당 실패");
		return false;
	}


	// ODBC 3.x 버전 설정
	ret = SQLSetEnvAttr(hEnv_, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
	if (!SQL_SUCCEEDED(ret))
	{
		_LogError_("SQLServer ODBC 버전 설정 실패 : %s", GetDiagnosticMessage(SQL_HANDLE_ENV, hEnv_).Source());
		SQLFreeHandle(SQL_HANDLE_ENV, hEnv_);
		hEnv_ = SQL_NULL_HENV;
		return false;
	}

	// 연결 핸들 할당
	ret = SQLAllocHandle(SQL_HANDLE_DBC, hEnv_, &hDbc_);
	if (!SQL_SUCCEEDED(ret))
	{
		_LogError_("SQLServer ODBC 연결 핸들 할당 실패 : %s", GetDiagnosticMessage(SQL_HANDLE_ENV, hEnv_).Source());
		SQLFreeHandle(SQL_HANDLE_ENV, hEnv_);
		hEnv_ = SQL_NULL_HENV;
		return false;
	}

	// 연결 타임아웃 설정 (5초)
	SQLSetConnectAttr(hDbc_, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0);

	// 연결 문자열 구성 (MARS_Connection=Yes: 커넥션 풀에서 커넥션 반환 후에도 SELECT 결과셋 안전하게 사용)
	// ODBC 드라이버 보는 방법
	// MARS_Connection은 SQL Server에서 다중 활성 결과 집합(Multiple Active Result Sets)을 허용하는 옵션입니다. 이
	// 옵션이 활성화되면 하나의 연결에서 여러 개의 결과 집합을 동시에 사용할 수 있습니다.
	char connStr[512];
	jc::StringUtil::FormatBuffer(connStr, 512,
		"DRIVER={ODBC Driver 17 for SQL Server};SERVER=%s,%d;DATABASE=%s;UID=%s;PWD=%s;MARS_Connection=Yes;",
		hostname_.Source(), static_cast<int>(port_), dbName_.Source(), username_.Source(), password_.Source());

	SQLCHAR outConnStr[1024];
	SQLSMALLINT outConnStrLen;

	ret = SQLDriverConnectA(hDbc_, NULL, (SQLCHAR*)connStr, SQL_NTS, outConnStr, sizeof(outConnStr), &outConnStrLen, SQL_DRIVER_NOPROMPT);

	if (!SQL_SUCCEEDED(ret))
	{
		isConnected_ = false;
		_LogError_("SQLServer 데이터베이스 연결 실패 : %s", GetDiagnosticMessage(SQL_HANDLE_DBC, hDbc_).Source());
		SQLFreeHandle(SQL_HANDLE_DBC, hDbc_);
		SQLFreeHandle(SQL_HANDLE_ENV, hEnv_);
		hDbc_ = SQL_NULL_HDBC;
		hEnv_ = SQL_NULL_HENV;
	}
	else
	{
		isConnected_ = true;
	}

	return isConnected_;
}

//////////////////////////////////////////////////////////////////////////////////////////
void SqlServerConnection::Disconnect()
{
	if (isConnected_)
	{
		SQLDisconnect(hDbc_);
		isConnected_ = false;
	}

	if (hDbc_ != SQL_NULL_HDBC)
	{
		SQLFreeHandle(SQL_HANDLE_DBC, hDbc_);
		hDbc_ = SQL_NULL_HDBC;
	}

	if (hEnv_ != SQL_NULL_HENV)
	{
		SQLFreeHandle(SQL_HANDLE_ENV, hEnv_);
		hEnv_ = SQL_NULL_HENV;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
bool SqlServerConnection::SelectDB(const jc::String& _dbName)
{
	if (!isConnected_)
	{
		_LogError_("SelectDB() 실패 : SQLServer 데이터베이스에 연결되어 있지 않습니다.");
		return false;
	}

	jc::String useDbStatement = jc::String("USE ") + _dbName;

	SQLHSTMT hStmt = SQL_NULL_HSTMT;
	SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_STMT, hDbc_, &hStmt);
	if (!SQL_SUCCEEDED(ret))
	{
		_LogError_("SelectDB() 실패 : 스테이트먼트 핸들 할당 실패");
		return false;
	}

	ret = SQLExecDirectA(hStmt, (SQLCHAR*)useDbStatement.Source(), SQL_NTS);
	bool success = SQL_SUCCEEDED(ret);

	if (!success)
	{
		_LogError_("SelectDB() 실패 : %s", GetDiagnosticMessage(SQL_HANDLE_STMT, hStmt).Source());
	}
	else
	{
		dbName_ = _dbName;
		_LogDebug_("SelectDB() 성공 : \"%s\"", _dbName.Source());
	}

	SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
	return success;
}

//////////////////////////////////////////////////////////////////////////////////////////
jc::String SqlServerConnection::GetLastErrorString() const
{
	if (!isConnected_)
	{
		_LogWarn_("GetLastErrorString() 실패 : SQLServer 데이터베이스에 연결되어 있지 않습니다.");
		return "연결 안되있음";
	}

	return GetDiagnosticMessage(SQL_HANDLE_DBC, hDbc_);
}

//////////////////////////////////////////////////////////////////////////////////////////
int SqlServerConnection::GetLastErrorCode() const
{
	if (!isConnected_)
	{
		_LogWarn_("GetLastErrorCode() 실패 : SQLServer 데이터베이스에 연결되어 있지 않습니다.");
		return -1;
	}

	return static_cast<int>(GetDiagnosticCode(SQL_HANDLE_DBC, hDbc_));
}

//////////////////////////////////////////////////////////////////////////////////////////
int SqlServerConnection::GetLastError(OUT jc::String& _str) const
{
	if (!isConnected_)
	{
		_LogWarn_("GetLastError() 실패 : SQLServer 데이터베이스에 연결되어 있지 않습니다.");
		_str = "연결 안되있음";
		return -2;
	}

	return GetDiagnostic(SQL_HANDLE_DBC, hDbc_, _str);
}

//////////////////////////////////////////////////////////////////////////////////////////
SQLHDBC SqlServerConnection::GetConnection() const
{
	return hDbc_;
}

//////////////////////////////////////////////////////////////////////////////////////////
SQLHENV SqlServerConnection::GetEnvironment() const
{
	return hEnv_;
}

//////////////////////////////////////////////////////////////////////////////////////////
jc::String SqlServerConnection::EscapeString(const jc::String& _value) const
{
	// SQL Server는 싱글 쿼트를 더블링하여 이스케이프 (N' 접두사로 유니코드 리터럴)
	jc::String escaped(_value.Length() * 2 + 4);

	escaped += "N'";

	for (int i = 0; i < _value.Length(); ++i)
	{
		char ch = _value[i];
		if (ch == '\'')
			escaped += "''";
		else
			escaped += ch;
	}

	escaped += "'";
	return escaped;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool SqlServerConnection::BeginTransaction()
{
	if (!isConnected_)
	{
		_LogError_("BeginTransaction() 실패 : SQLServer 데이터베이스에 연결되어 있지 않습니다.");
		return false;
	}

	// AutoCommit 해제하여 트랜잭션 시작
	SQLRETURN ret = SQLSetConnectAttr(hDbc_, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_OFF, 0);
	if (!SQL_SUCCEEDED(ret))
	{
		_LogError_("BeginTransaction() 실패 : %s", GetDiagnosticMessage(SQL_HANDLE_DBC, hDbc_).Source());
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool SqlServerConnection::Commit()
{
	if (!isConnected_)
	{
		_LogError_("Commit() 실패 : SQLServer 데이터베이스에 연결되어 있지 않습니다.");
		return false;
	}

	SQLRETURN ret = SQLEndTran(SQL_HANDLE_DBC, hDbc_, SQL_COMMIT);
	if (!SQL_SUCCEEDED(ret))
	{
		_LogError_("Commit() 실패 : %s", GetDiagnosticMessage(SQL_HANDLE_DBC, hDbc_).Source());
		return false;
	}

	// AutoCommit 복원
	SQLSetConnectAttr(hDbc_, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, 0);
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool SqlServerConnection::Rollback()
{
	if (!isConnected_)
	{
		_LogError_("Rollback() 실패 : SQLServer 데이터베이스에 연결되어 있지 않습니다.");
		return false;
	}

	SQLRETURN ret = SQLEndTran(SQL_HANDLE_DBC, hDbc_, SQL_ROLLBACK);
	if (!SQL_SUCCEEDED(ret))
	{
		_LogError_("Rollback() 실패 : %s", GetDiagnosticMessage(SQL_HANDLE_DBC, hDbc_).Source());
		return false;
	}

	// AutoCommit 복원
	SQLSetConnectAttr(hDbc_, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, 0);
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool SqlServerConnection::SetAutoCommit(bool _enable)
{
	if (!isConnected_)
	{
		_LogError_("SetAutoCommit() 실패 : SQLServer 데이터베이스에 연결되어 있지 않습니다.");
		return false;
	}

	SQLPOINTER value = _enable ? (SQLPOINTER)SQL_AUTOCOMMIT_ON : (SQLPOINTER)SQL_AUTOCOMMIT_OFF;
	SQLRETURN ret = SQLSetConnectAttr(hDbc_, SQL_ATTR_AUTOCOMMIT, value, 0);
	if (!SQL_SUCCEEDED(ret))
	{
		_LogError_("SetAutoCommit() 실패 : %s", GetDiagnosticMessage(SQL_HANDLE_DBC, hDbc_).Source());
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
int SqlServerConnection::GetDiagnostic(SQLSMALLINT _handleType, SQLHANDLE _handle, OUT jc::String& _msg) const
{
	SQLCHAR sqlState[6];
	SQLINTEGER nativeError = 0;
	SQLCHAR messageText[512];
	SQLSMALLINT textLength;
	SQLRETURN ret = SQLGetDiagRecA(_handleType, _handle, 1, sqlState, &nativeError, messageText, sizeof(messageText), &textLength);
	if (SQL_SUCCEEDED(ret))
	{
		_msg = jc::String((char*)messageText);
	}
	return nativeError;
}

//////////////////////////////////////////////////////////////////////////////////////////
jc::String SqlServerConnection::GetDiagnosticMessage(SQLSMALLINT _handleType, SQLHANDLE _handle) const
{
	SQLCHAR sqlState[6];
	SQLINTEGER nativeError;
	SQLCHAR messageText[512];
	SQLSMALLINT textLength;

	SQLRETURN ret = SQLGetDiagRecA(_handleType, _handle, 1, sqlState, &nativeError, messageText, sizeof(messageText), &textLength);

	if (SQL_SUCCEEDED(ret))
	{
		return jc::String((char*)messageText);
	}

	return "알 수 없는 오류";
}

//////////////////////////////////////////////////////////////////////////////////////////
SQLINTEGER SqlServerConnection::GetDiagnosticCode(SQLSMALLINT _handleType, SQLHANDLE _handle) const
{
	SQLCHAR sqlState[6];
	SQLINTEGER nativeError = 0;
	SQLCHAR messageText[512];
	SQLSMALLINT textLength;

	SQLGetDiagRecA(_handleType, _handle, 1, sqlState, &nativeError, messageText, sizeof(messageText), &textLength);

	return nativeError;
}

NS_END
