#include "SqlServerQuery.h"

#include "type_traits"
#include "cstring"

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
int SqlServerQuerySelect::GetColIndex(const char* _pFieldName)
{
	const int* pIndex = colNameMap_.Find(_pFieldName);

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

	const int fieldIndex = GetColIndex(_pFieldName);

	if (fieldIndex == -1)
	{
		_LogError_("%s 필드를 찾지 못했습니다. %s", _pFieldName, "GetRawString()");
		return nullptr;
	}

	return row_[fieldIndex].pBuf_;
}

//////////////////////////////////////////////////////////////////////////////////////////
const char* SqlServerQuerySelect::GetRawString(int _colIdx)
{
	if (IsFailed())
	{
		_LogError_("쿼리 수행결과가 존재하지 않습니다. %s", "GetRawString()");
		return nullptr;
	}

	if (_colIdx < 0 || _colIdx >= static_cast<int>(colCount_))
	{
		_LogError_("컬럼 인덱스(%d)가 범위를 벗어났습니다. (0~%d) %s",
			_colIdx, colCount_ - 1, "GetRawString()");
		return nullptr;
	}

	return row_[_colIdx].pBuf_;
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
	const int fieldIndex = GetColIndex(_pFieldName);
	if (fieldIndex == -1)
	{
		_LogError_("%s 필드를 찾지 못했습니다. GetDateTime()", _pFieldName);
		return 0;
	}
	return GetDateTime(fieldIndex);
}

//////////////////////////////////////////////////////////////////////////////////////////
DateTime SqlServerQuerySelect::GetDateTime(int _colIdx)
{
	if (IsFailed() || _colIdx < 0 || _colIdx >= static_cast<int>(colCount_))
		return 0;

	ColumnBinder& col = row_[_colIdx];
	if (col.length_ == SQL_NULL_DATA || col.length_ <= 0)
		return 0;

	const SQLSMALLINT sqlType = colInfoList_[_colIdx].type_;

	switch (sqlType)
	{
	case SQL_CHAR:
	case SQL_VARCHAR:
		{
			col.pBuf_[col.length_] = '\0';
			return ParseStringToDateTime(col.pBuf_);
		}
	case SQL_TYPE_TIMESTAMP:
	case SQL_DATETIME:
		{
			auto* pTs = reinterpret_cast<const SQL_TIMESTAMP_STRUCT*>(col.pBuf_);
			const _s32 milli = static_cast<_s32>(pTs->fraction / 1000000);
			const _s32 micro = static_cast<_s32>((pTs->fraction % 1000000) / 1000);
			return jc::DateAndTime(
				pTs->year, pTs->month, pTs->day,
				pTs->hour, pTs->minute, pTs->second,
				milli, micro
			).ToDateTime();
		}
	case SQL_TYPE_DATE:
		{
			auto* pDs = reinterpret_cast<const SQL_DATE_STRUCT*>(col.pBuf_);
			return jc::DateAndTime(pDs->year, pDs->month, pDs->day, 0, 0, 0, 0, 0).ToDateTime();
		}
	case SQL_TYPE_TIME:
		{
			auto* pTs2 = reinterpret_cast<const SQL_TIME_STRUCT*>(col.pBuf_);
			return jc::DateAndTime(0, 1, 1, pTs2->hour, pTs2->minute, pTs2->second, 0, 0).ToDateTime();
		}
	default:
		return ParseStringToDateTime(col.pBuf_);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
_u32 SqlServerQuerySelect::GetColCount() const
{
	if (IsFailed())
	{
		_LogError_("쿼리 수행결과가 존재하지 않습니다. GetFieldCount()");
		return 0;
	}

	return colNameMap_.Size();
}

//////////////////////////////////////////////////////////////////////////////////////////
int SqlServerQuerySelect::GetColType(int _colIdx) const
{
	if (_colIdx < 0 || _colIdx >= static_cast<int>(colCount_))
	{
		_LogError_("컬럼 인덱스(%d)가 범위를 벗어났습니다. (0~%d)", _colIdx, colCount_ - 1);
		return -1;
	}
	return colInfoList_[_colIdx].type_;
}

//////////////////////////////////////////////////////////////////////////////////////////
SQLULEN SqlServerQuerySelect::CalculateOptimalBufferSize_SQL_C_CHAR(SQLSMALLINT _sqlType, SQLULEN _columnSize)
{
	switch (_sqlType)
	{
	case SQL_CHAR:
	case SQL_VARCHAR:
		return _columnSize + 1;

	case SQL_WCHAR:
	case SQL_WVARCHAR:
		return (_columnSize + 1) * sizeof(SQLWCHAR);

	case SQL_GUID:
		return 37;

	case SQL_TINYINT:
	case SQL_SMALLINT:
	case SQL_INTEGER:
	case SQL_BIGINT:
	case SQL_REAL:
	case SQL_FLOAT:
	case SQL_DOUBLE:
	case SQL_NUMERIC:
	case SQL_DECIMAL:
		return 32; // safe numeric buffer

	case SQL_TYPE_DATE:
		return 16;

	case SQL_TYPE_TIME:
		return 16;

	case SQL_TYPE_TIMESTAMP:
		return 32;

	case SQL_LONGVARCHAR:
	case SQL_WLONGVARCHAR:
	case SQL_LONGVARBINARY:
		return COL_BUFFER_SIZE;

	default:
		return COL_BUFFER_SIZE;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
SqlServerQuerySelect::~SqlServerQuerySelect()
{
	int size = row_.Size();
	for (int i = 0; i < size; ++i)
	{
		JC_DELETE_ARRAY_SAFE(row_[i].pBuf_);
		row_[i].capacity_ = 0;
		row_[i].length_ = 0;
	}
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
	SQLNumResultCols(hStmt_, &colCount_);
	row_.Resize(colCount_);
	colInfoList_.Resize(colCount_);

	for (SQLSMALLINT i = 1; i <= colCount_; ++i)
	{
		SQLCHAR columnName[256];
		SQLSMALLINT nameLength = 0;
		SQLSMALLINT dataType = 0;
		SQLULEN columnSize = 0;
		SQLSMALLINT decimalDigits = 0;
		SQLSMALLINT nullable = 0;

		SQLDescribeColA(hStmt_, i, columnName, sizeof(columnName), &nameLength,
			&dataType, &columnSize, &decimalDigits, &nullable);

		int index = i - 1;
		jc::String fieldName((char*)columnName);

		ColumnBinder& col = row_[index];
		col.capacity_ = columnSize + 1;
		col.pBuf_ = dbg_new char[col.capacity_];
		col.pBuf_[columnSize] = '\0'; // 안전을 위해 널 종료
		
		SQLRETURN bindRet = SQLBindCol(hStmt_, i, SQL_C_BINARY, col.pBuf_, col.capacity_, &col.length_);
		if (!SQL_SUCCEEDED(bindRet))
		{
			col.pBuf_[0] = '\0';
			_LogWarn_("SQLServer SELECT 컬럼 바인딩 실패 (필드: %s)", fieldName.SafeSource());
		}

		colInfoList_[index].type_ = dataType;
		colInfoList_[index].name_ = Move(fieldName);
	}

	return Next();
}

// 아래 방식은 내부에서 처음부터 반복문 돌면서 행 수를 계산하기 때문에 성능이 좋지 않다. 어쩔 수 없지만 rowCount_ 정보를 미리 얻을 순 없다..
// 정적 커서를 이용한 전체 행 수 계산
// 마지막 행으로 이동하여 행 번호를 가져온다.
// ret = SQLFetchScroll(hStmt_, SQL_FETCH_LAST, 0);
// if (SQL_SUCCEEDED(ret))
// {
// 	SQLULEN rowNumber = 0;
// 	SQLGetStmtAttr(hStmt_, SQL_ATTR_ROW_NUMBER, &rowNumber, 0, NULL);
// 	rowCount_ = static_cast<_u32>(rowNumber);
// 
// 	// 첫 번째 행으로 복귀
// 	SQLFetchScroll(hStmt_, SQL_FETCH_FIRST, 0);
// 	hasCurrentRow_ = true;
// 	LoadCurrentRowData();
// }
// else
// {
// 	hasCurrentRow_ = false;
// 	rowCount_ = 0;
// }

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
	if (ret == SQL_NO_DATA)
	{
		hasCurrentRow_ = false;
		return false;
	}

	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
	{
		hasCurrentRow_ = true;
		++rowCount_;
		colReadOffset_ = 0; // 자동 초기화
		return true;
	}

	hasCurrentRow_ = false;
	ExtractError(hStmt_);
	_LogError_("SQLFetch failed: %s", errorMsg_.SafeSource());
	return false;
}

namespace
{
	jc::String ConvertString(char* _pBuf, SQLLEN _length, SQLSMALLINT _sqlType)
	{
		if (_length == SQL_NULL_DATA || _length <= 0)
			return jc::String::Empty;

		switch (_sqlType)
		{
		case SQL_CHAR:
			{
				// char타입은 0x20(공백)으로 패딩되므로, 널 종료 위치를 찾아서 문자열을 복사한다.
				int nullPos = -1;
				for (int i = 0; i < _length; ++i)
				{
					if (_pBuf[i] == 0x20)
					{
						nullPos = i;
						break;
					}
				}
				char* pNewBuf = dbg_new char[_length + 1];
				Memory::CopyUnsafe(pNewBuf, _pBuf, nullPos != -1 ? nullPos : static_cast<int>(_length));
				pNewBuf[nullPos != -1 ? nullPos : static_cast<int>(_length)] = '\0';

				jc::String str(0);
				str.ExchangeSource(pNewBuf, nullPos != -1 ? nullPos : static_cast<int>(_length));
				return str;
			}
		case SQL_VARCHAR:
			{
				char* pNewBuf = dbg_new char[_length + 1];
				Memory::CopyUnsafe(pNewBuf, _pBuf, static_cast<int>(_length));
				pNewBuf[_length] = '\0';
				jc::String str(0);
				str.ExchangeSource(pNewBuf, static_cast<int>(_length));
				return str;
			}
		case SQL_WVARCHAR:
			{
				wchar_t* pWBuf = reinterpret_cast<wchar_t*>(_pBuf);
				return jc::StringUtil::ToUtf8(pWBuf, _length / sizeof(wchar_t));
			}
		default:
			{
				_pBuf[_length] = '\0';
				jc_assert_msg(false, "문자열 변환을 지원하지 않는 SQL 타입입니다. SQL 타입: %d", _sqlType);
				return jc::String(_pBuf);
			}
		}
	}

	template <typename T>
	T ConvertAuto(char* _pBuf, SQLLEN _length, SQLSMALLINT _sqlType)
	{
		if (_length == SQL_NULL_DATA || _length <= 0)
			return T{};

		switch (_sqlType)
		{
		case SQL_BIT:
		case SQL_TINYINT:
			{
				_u8 v;
				memcpy(&v, _pBuf, sizeof(v));
				return static_cast<T>(v);
			}
		case SQL_SMALLINT:
			{
				_s16 v;
				memcpy(&v, _pBuf, sizeof(v));
				return static_cast<T>(v);
			}
		case SQL_INTEGER:
			{
				_s32 v;
				memcpy(&v, _pBuf, sizeof(v));
				return static_cast<T>(v);
			}
		case SQL_BIGINT:
			{
				_s64 v;
				memcpy(&v, _pBuf, sizeof(v));
				return static_cast<T>(v);
			}
		case SQL_REAL:
			{
				_f32 v;
				memcpy(&v, _pBuf, sizeof(v));
				return static_cast<T>(v);
			}
		case SQL_FLOAT:
		case SQL_DOUBLE:
			{
				_f64 v;
				memcpy(&v, _pBuf, sizeof(v));
				return static_cast<T>(v);
			}
		case SQL_NUMERIC:
		case SQL_DECIMAL:
			{
				auto* pNumeric = reinterpret_cast<const SQL_NUMERIC_STRUCT*>(_pBuf);
				_u64 val = 0;
				memcpy(&val, pNumeric->val, sizeof(val));

				if constexpr (std::is_floating_point_v<T>)
				{
					_f64 result = static_cast<_f64>(val);
					for (SQLSCHAR i = 0; i < pNumeric->scale; ++i)
						result /= 10.0;
					if (pNumeric->sign == 0)
						result = -result;
					return static_cast<T>(result);
				}
				else
				{
					_s64 result = static_cast<_s64>(val);
					for (SQLSCHAR i = 0; i < pNumeric->scale; ++i)
						result /= 10;
					if (pNumeric->sign == 0)
						result = -result;
					return static_cast<T>(result);
				}
			}
		default:
			{
				// 문자열 타입 (CHAR, VARCHAR 등): 널 종료 후 StringUtil::ToNumber 폴백
				_pBuf[_length] = '\0';
				return jc::StringUtil::ToNumber<T>(_pBuf);
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
jc::String SqlServerQuerySelect::GetString(int _colIdx)
{
	if (_colIdx < 0 || _colIdx >= static_cast<int>(colCount_))
		return 0;

	ColumnBinder& col = row_[_colIdx];
	return ConvertString(col.pBuf_, col.length_, colInfoList_[_colIdx].type_);
}

//////////////////////////////////////////////////////////////////////////////////////////
_s8 SqlServerQuerySelect::GetS8(int _colIdx)
{
	if (_colIdx < 0 || _colIdx >= static_cast<int>(colCount_))
		return 0;

	ColumnBinder& col = row_[_colIdx];
	return ConvertAuto<_s8>(col.pBuf_, col.length_, colInfoList_[_colIdx].type_);
}

//////////////////////////////////////////////////////////////////////////////////////////
_u8 SqlServerQuerySelect::GetU8(int _colIdx)
{
	if (_colIdx < 0 || _colIdx >= static_cast<int>(colCount_))
		return 0;

	ColumnBinder& col = row_[_colIdx];
	return ConvertAuto<_u8>(col.pBuf_, col.length_, colInfoList_[_colIdx].type_);
}

//////////////////////////////////////////////////////////////////////////////////////////
_s16 SqlServerQuerySelect::GetS16(int _colIdx)
{
	if (_colIdx < 0 || _colIdx >= static_cast<int>(colCount_))
		return 0;

	ColumnBinder& col = row_[_colIdx];
	return ConvertAuto<_s16>(col.pBuf_, col.length_, colInfoList_[_colIdx].type_);
}

//////////////////////////////////////////////////////////////////////////////////////////
_u16 SqlServerQuerySelect::GetU16(int _colIdx)
{
	if (_colIdx < 0 || _colIdx >= static_cast<int>(colCount_))
		return 0;

	ColumnBinder& col = row_[_colIdx];
	return ConvertAuto<_u16>(col.pBuf_, col.length_, colInfoList_[_colIdx].type_);
}

//////////////////////////////////////////////////////////////////////////////////////////
_s32 SqlServerQuerySelect::GetS32(int _colIdx)
{
	if (_colIdx < 0 || _colIdx >= static_cast<int>(colCount_))
		return 0;

	ColumnBinder& col = row_[_colIdx];
	return ConvertAuto<_s32>(col.pBuf_, col.length_, colInfoList_[_colIdx].type_);
}

//////////////////////////////////////////////////////////////////////////////////////////
_u32 SqlServerQuerySelect::GetU32(int _colIdx)
{
	if (_colIdx < 0 || _colIdx >= static_cast<int>(colCount_))
		return 0;

	ColumnBinder& col = row_[_colIdx];
	return ConvertAuto<_u32>(col.pBuf_, col.length_, colInfoList_[_colIdx].type_);
}

//////////////////////////////////////////////////////////////////////////////////////////
_s64 SqlServerQuerySelect::GetS64(int _colIdx)
{
	if (_colIdx < 0 || _colIdx >= static_cast<int>(colCount_))
		return 0;

	ColumnBinder& col = row_[_colIdx];
	return ConvertAuto<_s64>(col.pBuf_, col.length_, colInfoList_[_colIdx].type_);
}

//////////////////////////////////////////////////////////////////////////////////////////
_u64 SqlServerQuerySelect::GetU64(int _colIdx)
{
	if (_colIdx < 0 || _colIdx >= static_cast<int>(colCount_))
		return 0;

	ColumnBinder& col = row_[_colIdx];
	return ConvertAuto<_u64>(col.pBuf_, col.length_, colInfoList_[_colIdx].type_);
}

//////////////////////////////////////////////////////////////////////////////////////////
_f32 SqlServerQuerySelect::GetFloat(int _colIdx)
{
	if (_colIdx < 0 || _colIdx >= static_cast<int>(colCount_))
		return 0.0f;

	ColumnBinder& col = row_[_colIdx];
	return ConvertAuto<_f32>(col.pBuf_, col.length_, colInfoList_[_colIdx].type_);
}

//////////////////////////////////////////////////////////////////////////////////////////
_f64 SqlServerQuerySelect::GetDouble(int _colIdx)
{
	if (_colIdx < 0 || _colIdx >= static_cast<int>(colCount_))
		return 0.0;

	ColumnBinder& col = row_[_colIdx];
	return ConvertAuto<_f64>(col.pBuf_, col.length_, colInfoList_[_colIdx].type_);
}

//////////////////////////////////////////////////////////////////////////////////////////
jc::Date SqlServerQuerySelect::GetDate(int _colIdx)
{
	if (IsFailed() || _colIdx < 0 || _colIdx >= static_cast<int>(colCount_))
		return {};

	ColumnBinder& col = row_[_colIdx];
	if (col.length_ == SQL_NULL_DATA || col.length_ <= 0)
		return {};

	const SQLSMALLINT sqlType = colInfoList_[_colIdx].type_;

	switch (sqlType)
	{
	case SQL_DATETIME:
		{
			auto* pTs = reinterpret_cast<const SQL_TIMESTAMP_STRUCT*>(col.pBuf_);
			return jc::Date(pTs->year, pTs->month, pTs->day);
		}
	case SQL_TYPE_DATE:
		{
			auto* pDs = reinterpret_cast<const SQL_DATE_STRUCT*>(col.pBuf_);
			return jc::Date{ pDs->year, pDs->month, pDs->day };
		}
	default:
		{
			jc_assert_msg(false, "지원하지 않는 SQL 타입입니다. (SQL 타입: %d)", sqlType);
			return jc::Date{};
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
jc::Time SqlServerQuerySelect::GetTime(int _colIdx)
{
	if (IsFailed() || _colIdx < 0 || _colIdx >= static_cast<int>(colCount_))
		return {};

	ColumnBinder& col = row_[_colIdx];
	if (col.length_ == SQL_NULL_DATA || col.length_ <= 0)
		return {};

	const SQLSMALLINT sqlType = colInfoList_[_colIdx].type_;

	switch (sqlType)
	{
	case SQL_DATETIME:
		{
			auto* pTs = reinterpret_cast<const SQL_TIMESTAMP_STRUCT*>(col.pBuf_);
			return jc::Time{ pTs->hour, pTs->minute, pTs->second, static_cast<_s32>(pTs->fraction / 1000000), static_cast<_s32>((pTs->fraction % 1000000) / 1000) };
		}
	case SQL_TYPE_TIME:
		{
			auto* pDs = reinterpret_cast<const SQL_TIME_STRUCT*>(col.pBuf_);
			return jc::Time{ pDs->hour, pDs->minute, pDs->second, 0, 0 };
		}
	case SQL_SS_TIME2:
		{
			auto* pTime2 = reinterpret_cast<const SQL_SS_TIME2_STRUCT*>(col.pBuf_);
			_s16 mili = static_cast<_s16>(pTime2->fraction / 1000000);
			_s16 micro = static_cast<_s16>((pTime2->fraction % 1000000) / 1000);
			return jc::Time{ pTime2->hour, pTime2->minute, pTime2->second, mili, micro};
		}
	default:
		{
			jc_assert_msg(false, "지원하지 않는 SQL 타입입니다. (SQL 타입: %d)", sqlType);
			return {};
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
jc::StringView SqlServerQuerySelect::ReadRawString()
{
	if (colReadOffset_ >= static_cast<_u32>(colCount_))
	{
		jc_assert_msg(false, "컬럼을 모두 읽었습니다. 더 이상 읽을 컬럼이 없습니다.");
		return jc::StringView();
	}
	ColumnBinder& col = row_[colReadOffset_++];
	return jc::StringView(col.pBuf_);
}

//////////////////////////////////////////////////////////////////////////////////////////
jc::String SqlServerQuerySelect::ReadString()
{
	if (colReadOffset_ >= static_cast<_u32>(colCount_))
	{
		jc_assert_msg(false, "컬럼을 모두 읽었습니다. 더 이상 읽을 컬럼이 없습니다.");
		return jc::String();
	}
	_u32 colIdx = colReadOffset_++;
	ColumnBinder& col = row_[colIdx];
	return ConvertString(col.pBuf_, col.length_, colInfoList_[colIdx].type_);
}

//////////////////////////////////////////////////////////////////////////////////////////
_s8 SqlServerQuerySelect::ReaS8()
{
	return GetS8(static_cast<int>(colReadOffset_++));
}

//////////////////////////////////////////////////////////////////////////////////////////
_u8 SqlServerQuerySelect::ReadU8()
{
	return GetU8(static_cast<int>(colReadOffset_++));
}

//////////////////////////////////////////////////////////////////////////////////////////
_s16 SqlServerQuerySelect::ReadS16()
{
	return GetS16(static_cast<int>(colReadOffset_++));
}

//////////////////////////////////////////////////////////////////////////////////////////
_u16 SqlServerQuerySelect::ReadU16()
{
	return GetU16(static_cast<int>(colReadOffset_++));
}

//////////////////////////////////////////////////////////////////////////////////////////
_s32 SqlServerQuerySelect::ReadS32()
{
	return GetS32(static_cast<int>(colReadOffset_++));
}

//////////////////////////////////////////////////////////////////////////////////////////
_u32 SqlServerQuerySelect::ReadU32()
{
	return GetU32(static_cast<int>(colReadOffset_++));
}

//////////////////////////////////////////////////////////////////////////////////////////
_s64 SqlServerQuerySelect::ReadS64()
{
	return GetS64(static_cast<int>(colReadOffset_++));
}

//////////////////////////////////////////////////////////////////////////////////////////
_u64 SqlServerQuerySelect::ReadU64()
{
	return GetU64(static_cast<int>(colReadOffset_++));
}

//////////////////////////////////////////////////////////////////////////////////////////
_f32 SqlServerQuerySelect::ReadFloat()
{
	return GetFloat(static_cast<int>(colReadOffset_++));
}

//////////////////////////////////////////////////////////////////////////////////////////
_f64 SqlServerQuerySelect::ReadDouble()
{
	return GetDouble(static_cast<int>(colReadOffset_++));
}

//////////////////////////////////////////////////////////////////////////////////////////
DateTime SqlServerQuerySelect::ReadDateTime()
{
	return GetDateTime(static_cast<int>(colReadOffset_++));
}
