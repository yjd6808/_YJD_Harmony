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
int MysqlQuerySelect::GetColIndex(const char* _pFieldName)
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

	const int fieldIndex = GetColIndex(_pFieldName);

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
_u32 MysqlQuerySelect::GetColCount() const
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
	colReadOffset_ = 0;
	return sqlRow_ != nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
_s8 MysqlQuerySelect::GetS8(int _fieldIdx)
{
	const char* pRawString = GetRawString(_fieldIdx);
	if (pRawString == nullptr) return 0;
	return jc::StringUtil::ToNumber<_s8>(pRawString);
}

//////////////////////////////////////////////////////////////////////////////////////////
_u8 MysqlQuerySelect::GetU8(int _fieldIdx)
{
	const char* pRawString = GetRawString(_fieldIdx);
	if (pRawString == nullptr) return 0;
	return jc::StringUtil::ToNumber<_u8>(pRawString);
}

//////////////////////////////////////////////////////////////////////////////////////////
_s16 MysqlQuerySelect::GetS16(int _fieldIdx)
{
	const char* pRawString = GetRawString(_fieldIdx);
	if (pRawString == nullptr) return 0;
	return jc::StringUtil::ToNumber<_s16>(pRawString);
}

//////////////////////////////////////////////////////////////////////////////////////////
_u16 MysqlQuerySelect::GetU16(int _fieldIdx)
{
	const char* pRawString = GetRawString(_fieldIdx);
	if (pRawString == nullptr) return 0;
	return jc::StringUtil::ToNumber<_u16>(pRawString);
}

//////////////////////////////////////////////////////////////////////////////////////////
_s32 MysqlQuerySelect::GetS32(int _fieldIdx)
{
	const char* pRawString = GetRawString(_fieldIdx);
	if (pRawString == nullptr) return 0;
	return jc::StringUtil::ToNumber<_s32>(pRawString);
}

//////////////////////////////////////////////////////////////////////////////////////////
_u32 MysqlQuerySelect::GetU32(int _fieldIdx)
{
	const char* pRawString = GetRawString(_fieldIdx);
	if (pRawString == nullptr) return 0;
	return jc::StringUtil::ToNumber<_u32>(pRawString);
}

//////////////////////////////////////////////////////////////////////////////////////////
_s64 MysqlQuerySelect::GetS64(int _fieldIdx)
{
	const char* pRawString = GetRawString(_fieldIdx);
	if (pRawString == nullptr) return 0;
	return jc::StringUtil::ToNumber<_s64>(pRawString);
}

//////////////////////////////////////////////////////////////////////////////////////////
_u64 MysqlQuerySelect::GetU64(int _fieldIdx)
{
	const char* pRawString = GetRawString(_fieldIdx);
	if (pRawString == nullptr) return 0;
	return jc::StringUtil::ToNumber<_u64>(pRawString);
}

//////////////////////////////////////////////////////////////////////////////////////////
_f32 MysqlQuerySelect::GetFloat(int _fieldIdx)
{
	const char* pRawString = GetRawString(_fieldIdx);
	if (pRawString == nullptr) return 0.0f;
	return jc::StringUtil::ToNumber<_f32>(pRawString);
}

//////////////////////////////////////////////////////////////////////////////////////////
_f64 MysqlQuerySelect::GetDouble(int _fieldIdx)
{
	const char* pRawString = GetRawString(_fieldIdx);
	if (pRawString == nullptr) return 0.0;
	return jc::StringUtil::ToNumber<_f64>(pRawString);
}

//////////////////////////////////////////////////////////////////////////////////////////
jc::StringView MysqlQuerySelect::ReadRawString()
{
	const char* pRaw = GetRawString(static_cast<int>(colReadOffset_++));
	return pRaw ? jc::StringView(pRaw) : jc::StringView();
}

//////////////////////////////////////////////////////////////////////////////////////////
jc::String MysqlQuerySelect::ReadString()
{
	const char* pRaw = GetRawString(static_cast<int>(colReadOffset_++));
	return pRaw ? jc::String(pRaw) : jc::String();
}

//////////////////////////////////////////////////////////////////////////////////////////
_s8 MysqlQuerySelect::ReaS8()
{
	return GetS8(static_cast<int>(colReadOffset_++));
}

//////////////////////////////////////////////////////////////////////////////////////////
_u8 MysqlQuerySelect::ReadU8()
{
	return GetU8(static_cast<int>(colReadOffset_++));
}

//////////////////////////////////////////////////////////////////////////////////////////
_s16 MysqlQuerySelect::ReadS16()
{
	return GetS16(static_cast<int>(colReadOffset_++));
}

//////////////////////////////////////////////////////////////////////////////////////////
_u16 MysqlQuerySelect::ReadU16()
{
	return GetU16(static_cast<int>(colReadOffset_++));
}

//////////////////////////////////////////////////////////////////////////////////////////
_s32 MysqlQuerySelect::ReadS32()
{
	return GetS32(static_cast<int>(colReadOffset_++));
}

//////////////////////////////////////////////////////////////////////////////////////////
_u32 MysqlQuerySelect::ReadU32()
{
	return GetU32(static_cast<int>(colReadOffset_++));
}

//////////////////////////////////////////////////////////////////////////////////////////
_s64 MysqlQuerySelect::ReadS64()
{
	return GetS64(static_cast<int>(colReadOffset_++));
}

//////////////////////////////////////////////////////////////////////////////////////////
_u64 MysqlQuerySelect::ReadU64()
{
	return GetU64(static_cast<int>(colReadOffset_++));
}

//////////////////////////////////////////////////////////////////////////////////////////
_f32 MysqlQuerySelect::ReadFloat()
{
	return GetFloat(static_cast<int>(colReadOffset_++));
}

//////////////////////////////////////////////////////////////////////////////////////////
_f64 MysqlQuerySelect::ReadDouble()
{
	return GetDouble(static_cast<int>(colReadOffset_++));
}

//////////////////////////////////////////////////////////////////////////////////////////
jc::DateTime MysqlQuerySelect::ReadDateTime()
{
	return GetDateTime(static_cast<int>(colReadOffset_++));
}

NS_END