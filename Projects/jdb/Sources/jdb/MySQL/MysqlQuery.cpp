#include "Core.h"
#include "MysqlQuery.h"

USING_NS_JC;
USING_NS_STD;

NS_JDB_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
bool MysqlQueryUpdate::Execute()
{
	auto pMysqlConn = AsMysqlConn(pConn_);

	if (mysql_query(pMysqlConn->GetConnection(), ptmt_.Source()))
	{
		const String errorString = pMysqlConn->GetLastErrorString();
		errorCode_ = pMysqlConn->GetLastErrorCode();
		if (errorString.Length() > 2)
			_LogError_("MySQL UPDATE 오류 : %s", errorString.Source());
		return false;
	}

	rowCount_ = static_cast<_u32>(mysql_affected_rows(pMysqlConn->GetConnection()));
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool MysqlQueryDelete::Execute()
{
	auto pMysqlConn = AsMysqlConn(pConn_);

	if (mysql_query(pMysqlConn->GetConnection(), ptmt_.Source()))
	{
		const String errorString = pMysqlConn->GetLastErrorString();
		errorCode_ = pMysqlConn->GetLastErrorCode();
		if (errorString.Length() > 2)
			_LogError_("MySQL DELETE 오류 : %s", errorString.Source());
		return false;
	}

	rowCount_ = static_cast<_u32>(mysql_affected_rows(pMysqlConn->GetConnection()));
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool MysqlQueryInsert::Execute()
{
	auto pMysqlConn = AsMysqlConn(pConn_);

	if (mysql_query(pMysqlConn->GetConnection(), ptmt_.Source()))
	{
		const String errorString = pMysqlConn->GetLastErrorString();
		errorCode_ = pMysqlConn->GetLastErrorCode();
		if (errorString.Length() > 2)
			_LogError_("MySQL INSERT 오류 : %s", errorString.Source());
		return false;
	}

	rowCount_  = static_cast<_u32>(mysql_affected_rows(pMysqlConn->GetConnection()));
	// https://dev.mysql.com/doc/c-api/5.7/en/mysql-insert-id.html
	insertId_  = mysql_insert_id(pMysqlConn->GetConnection());
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
int MysqlQuerySelect::GetFieldIndex(const char* _pFieldName)
{
	const int* pIndex = fieldList_.Find(_pFieldName);

	if (pIndex == nullptr)
		return -1;

	return *pIndex;
}

//////////////////////////////////////////////////////////////////////////////////////////
const char* MysqlQuerySelect::GetRawString(const char* _pFieldName)
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

	return sqlRow_[fieldIndex];
}

//////////////////////////////////////////////////////////////////////////////////////////
const char* MysqlQuerySelect::GetRawString(int _fieldIndex)
{
	if (IsFailed())
	{
		_LogError_("쿼리 수행결과가 존재하지 않습니다. %s", "GetRawString()");
		return nullptr;
	}

	if (sqlRow_ == nullptr)
		return nullptr;

	if (_fieldIndex < 0 || _fieldIndex >= static_cast<int>(fieldList_.Size()))
	{
		_LogError_("필드 인덱스(%d)가 범위를 벗어났습니다. (0~%d) %s",
		           _fieldIndex, static_cast<int>(fieldList_.Size()) - 1, "GetRawString()");
		return nullptr;
	}

	return sqlRow_[_fieldIndex];
}

//////////////////////////////////////////////////////////////////////////////////////////
jc::DateTime MysqlQuerySelect::ParseRawStringToDateTime(const char* _pRawString)
{
	static constexpr const char* DECIMAL_POINT_FORMATS[]{
		"",
		".f",
		".ff",
		".fff",
		".ffff",
		".fffff",
		".ffffff"
	};

	static constexpr char DATE_FORMAT[64] = "yyyy-MM-dd HH:mm:ss%s";

	if (_pRawString == nullptr)
		return 0;

	DateTime parsed;

	char dateFormatBuffer[64];
	int decimalPointPos = StringUtil::FindCharReverse(_pRawString, '.');
	int decimalPlaceCount = 0;

	if (decimalPointPos != -1)
	{
		while (_pRawString[++decimalPointPos] != NULL)
		{
			decimalPlaceCount++;
		}

		jc_assert_msg(decimalPlaceCount > 0, "소수점(.)이 있는데 소수점 자릿수가 하나도 없습니다.");
	}

	if (decimalPlaceCount > 6)
		decimalPlaceCount = 6;

	StringUtil::FormatBuffer(dateFormatBuffer, 64, DATE_FORMAT, DECIMAL_POINT_FORMATS[decimalPlaceCount]);
	DateTime::TryParse(parsed, dateFormatBuffer, _pRawString);
	jc_assert_msg(DateTime::LastError() == 0, "소수점 날짜 포맷 파싱수행중 오류가 발생하였습니다. (%s)", DateTime::LastErrorMessage());
	return parsed;
}

//////////////////////////////////////////////////////////////////////////////////////////
DateTime MysqlQuerySelect::GetDateTime(const char* _pFieldName)
{
	const char* pRawString = GetRawString(_pFieldName);
	return ParseRawStringToDateTime(pRawString);
}

//////////////////////////////////////////////////////////////////////////////////////////
DateTime MysqlQuerySelect::GetDateTime(int _fieldIndex)
{
	const char* pRawString = GetRawString(_fieldIndex);
	return ParseRawStringToDateTime(pRawString);
}

//////////////////////////////////////////////////////////////////////////////////////////
_u32 MysqlQuerySelect::GetFieldCount() const
{
	if (IsFailed())
	{
		_LogError_("쿼리 수행결과가 존재하지 않습니다. GetFieldCount()");
		return 0;
	}

	return fieldList_.Size();
}

//////////////////////////////////////////////////////////////////////////////////////////
MysqlQuerySelect::~MysqlQuerySelect()
{
	if (sqlResult_)
	{
		mysql_free_result(sqlResult_);
		sqlResult_ = nullptr;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
bool MysqlQuerySelect::Execute()
{
	auto pMysqlConn = AsMysqlConn(pConn_);

	if (mysql_query(pMysqlConn->GetConnection(), ptmt_.Source()))
	{
		const String errorString = pMysqlConn->GetLastErrorString();
		errorCode_ = pMysqlConn->GetLastErrorCode();
		if (errorString.Length() > 2)
			_LogError_("MySQL SELECT 오류 : %s", errorString.Source());
		return false;
	}

	sqlResult_ = mysql_store_result(pMysqlConn->GetConnection());

	if (sqlResult_ == nullptr)
	{
		const String errorString = pMysqlConn->GetLastErrorString();
		errorCode_ = pMysqlConn->GetLastErrorCode();
		if (errorString.Length() > 2)
			_LogError_("MySQL SELECT 오류 : %s", errorString.Source());
		return false;
	}

	MYSQL_FIELD* pField;
	int fieldIndex = 0;
	while ((pField = mysql_fetch_field(sqlResult_)))
	{
		fieldList_.Insert(pField->name, fieldIndex);
		fieldIndex++;
	}

	sqlRow_ = mysql_fetch_row(sqlResult_);
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool MysqlQuerySelect::HasNext() const
{
	return sqlRow_ != nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool MysqlQuerySelect::Next()
{
	sqlRow_ = mysql_fetch_row(sqlResult_);
	return sqlRow_ != nullptr;
}

NS_END