#include "Core.h"
#include "SqlServerQuery.h"

USING_NS_JC;
USING_NS_STD;
USING_NS_JDB;

//////////////////////////////////////////////////////////////////////////////////////////
SqlServerQuery::~SqlServerQuery()
{
	FreeStatement();
}

//////////////////////////////////////////////////////////////////////////////////////////
void SqlServerQuery::ExtractError(SQLHSTMT _hStmt)
{
	SQLCHAR sqlState[6];
	SQLINTEGER nativeError = 0;
	SQLCHAR messageText[512];
	SQLSMALLINT textLength;

	SQLRETURN ret = SQLGetDiagRecA(SQL_HANDLE_STMT, _hStmt, 1, sqlState, &nativeError,
	                               messageText, sizeof(messageText), &textLength);

	if (SQL_SUCCEEDED(ret))
	{
		errorCode_ = static_cast<int>(nativeError);
		errorMsg_ = jc::String((char*)messageText);
	}
	else
	{
		errorCode_ = -1;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void SqlServerQuery::FreeStatement()
{
	if (hStmt_ != SQL_NULL_HSTMT)
	{
		SQLFreeHandle(SQL_HANDLE_STMT, hStmt_);
		hStmt_ = SQL_NULL_HSTMT;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
bool SqlServerQueryUpdate::Execute()
{
	auto pSqlServerConn = AsSqlServerConn(pConn_);

	SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_STMT, pSqlServerConn->GetConnection(), &hStmt_);
	if (!SQL_SUCCEEDED(ret))
	{
		errorCode_ = -1;
		_LogError_("SQLServer UPDATE 스테이트먼트 핸들 할당 실패");
		return false;
	}

	ret = SQLExecDirectA(hStmt_, (SQLCHAR*)ptmt_.SafeSource(), SQL_NTS);
	if (!SQL_SUCCEEDED(ret))
	{
		ExtractError(hStmt_);
		_LogError_("SQLServer UPDATE 오류 (errorCode=%d, %s)", errorCode_, errorMsg_.SafeSource());
		return false;
	}

	SQLLEN rowCount = 0;
	SQLRowCount(hStmt_, &rowCount);
	rowCount_ = static_cast<_u32>(rowCount);
	FreeStatement(); // 빨리 반환.
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool SqlServerQueryDelete::Execute()
{
	auto pSqlServerConn = AsSqlServerConn(pConn_);

	SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_STMT, pSqlServerConn->GetConnection(), &hStmt_);
	if (!SQL_SUCCEEDED(ret))
	{
		errorCode_ = -1;
		_LogError_("SQLServer DELETE 스테이트먼트 핸들 할당 실패");
		return false;
	}

	ret = SQLExecDirectA(hStmt_, (SQLCHAR*)ptmt_.SafeSource(), SQL_NTS);
	if (!SQL_SUCCEEDED(ret))
	{
		ExtractError(hStmt_);
		_LogError_("SQLServer DELETE 오류 (errorCode=%d, %s)", errorCode_, errorMsg_.SafeSource());
		return false;
	}

	SQLLEN rowCount = 0;
	SQLRowCount(hStmt_, &rowCount);
	rowCount_ = static_cast<_u32>(rowCount);
	FreeStatement(); // 빨리 반환.
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool SqlServerQueryInsert::Execute()
{
	auto pSqlServerConn = AsSqlServerConn(pConn_);

	SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_STMT, pSqlServerConn->GetConnection(), &hStmt_);
	if (!SQL_SUCCEEDED(ret))
	{
		errorCode_ = -1;
		_LogError_("SQLServer INSERT 스테이트먼트 핸들 할당 실패");
		return false;
	}

	ret = SQLExecDirectA(hStmt_, (SQLCHAR*)ptmt_.SafeSource(), SQL_NTS);
	if (!SQL_SUCCEEDED(ret))
	{
		FreeStatement();
		ExtractError(hStmt_);
		_LogError_("SQLServer INSERT 오류 (errorCode=%d, %s)", errorCode_, errorMsg_.SafeSource());
		return false;
	}

	SQLLEN rowCount = 0;
	SQLRowCount(hStmt_, &rowCount);
	rowCount_ = static_cast<_u32>(rowCount);

	// SCOPE_IDENTITY()로 마지막 삽입 ID를 가져온다
	SQLHSTMT hIdentityStmt = SQL_NULL_HSTMT;
	ret = SQLAllocHandle(SQL_HANDLE_STMT, pSqlServerConn->GetConnection(), &hIdentityStmt);
	if (SQL_SUCCEEDED(ret))
	{
		ret = SQLExecDirectA(hIdentityStmt, (SQLCHAR*)"SELECT SCOPE_IDENTITY()", SQL_NTS);
		if (SQL_SUCCEEDED(ret))
		{
			ret = SQLFetch(hIdentityStmt);
			if (SQL_SUCCEEDED(ret))
			{
				SQLCHAR idBuffer[64];
				SQLLEN indicator;
				ret = SQLGetData(hIdentityStmt, 1, SQL_C_CHAR, idBuffer, sizeof(idBuffer), &indicator);
				if (SQL_SUCCEEDED(ret) && indicator != SQL_NULL_DATA)
					insertId_ = jc::StringUtil::ToNumber<_u64>((char*)idBuffer);
			}
		}
		SQLFreeHandle(SQL_HANDLE_STMT, hIdentityStmt);
	}
	FreeStatement(); // 빨리 반환.
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
int SqlServerQuerySelect::GetFieldIndex(const char* _pFieldName)
{
	const int* pIndex = fieldList_.Find(_pFieldName);

	if (pIndex == nullptr)
		return -1;

	return *pIndex;
}

//////////////////////////////////////////////////////////////////////////////////////////
const char* SqlServerQuerySelect::GetRawString(const char* _pFieldName)
{
	if (IsFailed())
	{
		_LogError_("쿼리 수행결과가 존재하지 않습니다. %s", "GetRawString()");
		return nullptr;
	}

	const int fieldIndex = GetFieldIndex(_pFieldName);

	if (fieldIndex == -1)
	{
		_LogError_("%s 필드를 찾지 못했습니다. %s", _pFieldName, "GetRawString()");
		return nullptr;
	}

	const jc::String* pData = nullptr;
	if (fieldIndex >= 0 && fieldIndex < rowData_.Size())
		pData = &rowData_[fieldIndex];

	if (pData == nullptr || pData->IsEmpty())
		return nullptr;

	return pData->Source();
}

//////////////////////////////////////////////////////////////////////////////////////////
const char* SqlServerQuerySelect::GetRawString(int _fieldIndex)
{
	if (IsFailed())
	{
		_LogError_("쿼리 수행결과가 존재하지 않습니다. %s", "GetRawString()");
		return nullptr;
	}

	if (_fieldIndex < 0 || _fieldIndex >= static_cast<int>(columnCount_))
	{
		_LogError_("필드 인덱스(%d)가 범위를 벗어났습니다. (0~%d) %s",
		           _fieldIndex, columnCount_ - 1, "GetRawString()");
		return nullptr;
	}

	const jc::String* pData = nullptr;
	if (_fieldIndex >= 0 && _fieldIndex < rowData_.Size())
		pData = &rowData_[_fieldIndex];

	if (pData == nullptr || pData->IsEmpty())
		return nullptr;

	return pData->Source();
}

//////////////////////////////////////////////////////////////////////////////////////////
DateTime SqlServerQuerySelect::ParseStringToDateTime(const char* _pRawString)
{
	static constexpr const char* DECIMAL_POINT_FORMATS[]{
		"",
		".f",
		".ff",
		".fff",
		".ffff",
		".fffff",
		".ffffff",
		".fffffff"
	};

	// SQL Server datetime2 포맷: yyyy-MM-dd HH:mm:ss.fffffff
	static constexpr char DATE_FORMAT[64] = "yyyy-MM-dd HH:mm:ss%s";

	if (_pRawString == nullptr)
		return 0;

	DateTime parsed;

	char dateFormatBuffer[64];
	int decimalPointPos = StringUtil::FindCharReverse(_pRawString, '.');
	int decimalPlaceCount = 0;

	if (decimalPointPos != -1)
	{
		int floatIndex = decimalPointPos + 1;
		for (;;)
		{
			char ch = _pRawString[floatIndex];
			if (ch == NULL) break;
			++floatIndex;
		}

		decimalPlaceCount = floatIndex - decimalPointPos - 1;
		jc_assert_msg(decimalPlaceCount > 0, "소수점(.)이 있는데 소수점 자릿수가 하나도 없습니다.");
	}

	// SQL Server는 최대 7자리 소수점(datetime2)
	if (decimalPlaceCount >= 7)
		decimalPlaceCount = 6; // 내가 구현한 DateTime은 6자리 까지만 지원한다.

	StringUtil::FormatBuffer(dateFormatBuffer, 64, DATE_FORMAT, DECIMAL_POINT_FORMATS[decimalPlaceCount]);
	DateTime::TryParse(parsed, dateFormatBuffer, _pRawString);
	jc_assert_msg(DateTime::LastError() == 0, "소수점 날짜 포맷 파싱수행중 오류가 발생하였습니다. (%s)", DateTime::LastErrorMessage());
	return parsed;
}

//////////////////////////////////////////////////////////////////////////////////////////
DateTime SqlServerQuerySelect::GetDateTime(const char* _pFieldName)
{
	const char* pRawString = GetRawString(_pFieldName);
	return ParseStringToDateTime(pRawString);
}

//////////////////////////////////////////////////////////////////////////////////////////
DateTime SqlServerQuerySelect::GetDateTime(int _fieldIndex)
{
	const char* pRawString = GetRawString(_fieldIndex);
	return ParseStringToDateTime(pRawString);
}

//////////////////////////////////////////////////////////////////////////////////////////
_u32 SqlServerQuerySelect::GetFieldCount() const
{
	if (IsFailed())
	{
		_LogError_("쿼리 수행결과가 존재하지 않습니다. GetFieldCount()");
		return 0;
	}

	return fieldList_.Size();
}

//////////////////////////////////////////////////////////////////////////////////////////
bool SqlServerQuerySelect::Execute()
{
	auto pSqlServerConn = AsSqlServerConn(pConn_);

	SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_STMT, pSqlServerConn->GetConnection(), &hStmt_);
	if (!SQL_SUCCEEDED(ret))
	{
		errorCode_ = -1;
		_LogError_("SQLServer SELECT 스테이트먼트 핸들 할당 실패");
		return false;
	}

	// 정적 커서 설정: 결과를 클라이언트 측에 버퍼링 (mysql_store_result과 동일한 효과)
	// 커넥션 풀로 커넥션 반환 후에도 결과셋을 안전하게 사용할 수 있다.
	SQLSetStmtAttr(hStmt_, SQL_ATTR_CURSOR_TYPE, (SQLPOINTER)SQL_CURSOR_STATIC, 0);

	ret = SQLExecDirectA(hStmt_, (SQLCHAR*)ptmt_.SafeSource(), SQL_NTS);
	if (!SQL_SUCCEEDED(ret))
	{
		FreeStatement();
		ExtractError(hStmt_);
		_LogError_("SQLServer SELECT 오류 (errorCode=%d, %s)", errorCode_, errorMsg_.SafeSource());
		return false;
	}

	// 필드 정보 수집
	SQLNumResultCols(hStmt_, &columnCount_);

	for (SQLSMALLINT i = 1; i <= columnCount_; ++i)
	{
		SQLCHAR columnName[256];
		SQLSMALLINT nameLength;
		SQLSMALLINT dataType;
		SQLULEN columnSize;
		SQLSMALLINT decimalDigits;
		SQLSMALLINT nullable;

		SQLDescribeColA(hStmt_, i, columnName, sizeof(columnName), &nameLength,
		                &dataType, &columnSize, &decimalDigits, &nullable);
		int index = static_cast<int>(i - 1);
		jc::String fieldName((char*)columnName);
		fieldList_.Insert(fieldName, index);
	}

	// 정적 커서를 이용한 전체 행 수 계산
	// 마지막 행으로 이동하여 행 번호를 가져온다.
	ret = SQLFetchScroll(hStmt_, SQL_FETCH_LAST, 0);
	if (SQL_SUCCEEDED(ret))
	{
		SQLULEN rowNumber = 0;
		SQLGetStmtAttr(hStmt_, SQL_ATTR_ROW_NUMBER, &rowNumber, 0, NULL);
		rowCount_ = static_cast<_u32>(rowNumber);

		// 첫 번째 행으로 복귀
		SQLFetchScroll(hStmt_, SQL_FETCH_FIRST, 0);
		hasCurrentRow_ = true;
		LoadCurrentRowData();
	}
	else
	{
		hasCurrentRow_ = false;
		rowCount_ = 0;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool SqlServerQuerySelect::HasNext() const
{
	return hasCurrentRow_;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool SqlServerQuerySelect::Next()
{
	if (hStmt_ == SQL_NULL_HSTMT)
		return false;

	SQLRETURN ret = SQLFetch(hStmt_);
	if (SQL_SUCCEEDED(ret))
	{
		hasCurrentRow_ = true;
		LoadCurrentRowData();
		return true;
	}

	hasCurrentRow_ = false;
	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
void SqlServerQuerySelect::LoadCurrentRowData()
{
	rowData_.Clear();
	rowData_.Resize(columnCount_);

	SQLCHAR buffer[4096];
	SQLLEN len = SQL_NULL_DATA;

	for (SQLSMALLINT i = 1; i <= columnCount_; ++i)
	{
		SQLRETURN ret = SQLGetData(hStmt_, i, SQL_C_CHAR, buffer, sizeof(buffer), &len);
		if (SQL_SUCCEEDED(ret) && len != SQL_NULL_DATA)
			rowData_[i - 1] = jc::String((char*)buffer);
		else
			rowData_[i - 1] = jc::String(0);
	}
}
