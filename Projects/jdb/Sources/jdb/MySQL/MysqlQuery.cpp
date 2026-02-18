#include "Core.h"
#include "MysqlQuery.h"

USING_NS_JC;
USING_NS_STD;

NS_JDB_BEGIN

MysqlQuery::MysqlQuery(MysqlConnection* _pConn, const String& _preparedStatement, StatementType _type)
: statementType_(_type)
, conn_(_pConn)
, preparedStatement_(_preparedStatement)
, errorCode_(0)
{
}

StatementType MysqlQuery::ParseStatement(const String& _statement)
{
	StatementType statement = StatementType::None;
	const String statementPrefix = _statement.GetRange(0, 5).ToLowerCase(); // select, update, unsert, delete 모두 6글자이므로

	// 각 스테이트먼트가 각 문자열로 시작하는지 확인
	if (statementPrefix.Find("select") == 0)
	{
		statement = StatementType::Select;
	}
	else if (statementPrefix.Find("update") == 0)
	{
		statement = StatementType::Update;
	}
	else if (statementPrefix.Find("insert") == 0)
	{
		statement = StatementType::Insert;
	}
	else if (statementPrefix.Find("delete") == 0)
	{
		statement = StatementType::Delete;
	}
	else
	{
		jc_assert_msg(false, "올바른 스테이트먼트가 아닙니다.");
	}


	return statement;
}

Int32U MysqlQuery::GetRowCount() const
{
	if (!IsSuccess())
	{
		_LogError_("%s 쿼리 수행이 실패하여 영향받은 행 갯수를 가져오지 못했습니다.", StatementName(statementType_));
		return 0;
	}

	return (Int32U)mysql_affected_rows(conn_->GetConnection());
}

bool MysqlQueryUpdate::Execute()
{
	if (mysql_query(conn_->GetConnection(), preparedStatement_.Source()))
	{
		const String errorString = conn_->GetLastErrorString();
		errorCode_ = conn_->GetLastErrorCode();
		if (errorString.Length() > 2)
			_LogError_("MySQL UPDATE 오류 : %s", conn_->GetLastErrorString().Source());
		return false;
	}

	return true;
}

bool MysqlQueryDelete::Execute()
{
	if (mysql_query(conn_->GetConnection(), preparedStatement_.Source()))
	{
		const String errorString = conn_->GetLastErrorString();
		errorCode_ = conn_->GetLastErrorCode();
		if (errorString.Length() > 2)
			_LogError_("MySQL DELETE 오류 : %s", conn_->GetLastErrorString().Source());
		return false;
	}

	return true;
}

bool MysqlQueryInsert::Execute()
{
	if (mysql_query(conn_->GetConnection(), preparedStatement_.Source()))
	{
		const String errorString = conn_->GetLastErrorString();
		errorCode_ = conn_->GetLastErrorCode();
		if (errorString.Length() > 2)
			_LogError_("MySQL ISNERT 오류 : %s", conn_->GetLastErrorString().Source());
		return false;
	}

	// https://dev.mysql.com/doc/c-api/5.7/en/mysql-insert-id.html
	insertId_ = mysql_insert_id(conn_->GetConnection());
	return true;
}


int MysqlQuerySelect::GetFieldIndex(const char* _pFieldName)
{
	const int* pIndex = fieldList_.Find(_pFieldName);

	if (pIndex == nullptr)
	{
		return -1;
	}

	return *pIndex;
}

// 필드 값이 존재하지 않는 경우 nullptr이 반환될 수 있음
const char* MysqlQuerySelect::GetRawString(const char* _pFieldName)
{
	if (!IsSuccess())
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


String MysqlQuerySelect::GetString(const char* _pFieldName)
{
	return GetRawString(_pFieldName);
}

DateTime MysqlQuerySelect::GetDateTime(const char* _pFieldName)
{
	// TryParse 소수점 파싱 구현을 좀 범용성있게 했기땜에 ".ffffff"와 "" 두개로만 구분해도 무방함.
	// 일단 처음 구현할때 자릿수별로 세분화했기 땜에 그냥 두도록 한다.
	// TODO: GetDateTime 추후 좀더 간결하게 수정할 것
	static constexpr const char* DECIMAL_POINT_FORMATS[]{
		"",
		".f",
		".ff",
		".fff",
		".ffff",
		".fffff",
		".ffffff"
	};

	// Mysql 날짜형식 포맷
	static constexpr char DATE_FORMAT[64] = "yyyy-MM-dd HH:mm:ss%s";

	const char* pRawString = GetRawString(_pFieldName);
	if (pRawString == nullptr) return 0;
	DateTime parsed;


	char dateFormatBuffer[64];
	int decimalPointPos = StringUtil::FindCharReverse(pRawString, '.');
	int decimalPlaceCount = 0;

	// 예를들어 12:34:56.1234라는 시각 문자열정보가 있을 때 iDecimalPos는 소수점(.)의 위치를 의미
	// iDecimalPlaceCount = 소수점 자릿수, 4를 나타낸다.
	// 아래코드는 소수점 자릿수를 계산하는 코드를 구현한거다.

	if (decimalPointPos != -1)
	{
		while (pRawString[++decimalPointPos] != NULL)
		{
			decimalPlaceCount++;
		}

		jc_assert_msg(decimalPlaceCount > 0, "소수점(.)이 있는데 소수점 자릿수가 하나도 없습니다.");
	}

	StringUtil::FormatBuffer(dateFormatBuffer, 64, DATE_FORMAT, DECIMAL_POINT_FORMATS[decimalPlaceCount]);
	DateTime::TryParse(parsed, dateFormatBuffer, pRawString);
	jc_assert_msg(DateTime::LastError() == 0, "소수점 날짜 포맷 파싱수행중 오류가 발생하였습니다. (%s)", DateTime::LastErrorMessage());
	return parsed;
}

Int32U MysqlQuerySelect::GetRowCount() const
{
	if (!IsSuccess())
	{
		_LogError_("쿼리 수행결과가 존재하지 않습니다. GetFieldCount()");
		return 0;
	}

	return (Int32U)mysql_num_rows(sqlResult_);
}

Int32U MysqlQuerySelect::GetFieldCount() const
{
	if (!IsSuccess())
	{
		_LogError_("쿼리 수행결과가 존재하지 않습니다. GetFieldCount()");
		return 0;
	}

	return fieldList_.Size();
}


MysqlQuerySelect::~MysqlQuerySelect()
{
	if (sqlResult_)
	{
		mysql_free_result(sqlResult_);
		sqlResult_ = nullptr;
	}
}

bool MysqlQuerySelect::Execute()
{
	if (mysql_query(conn_->GetConnection(), preparedStatement_.Source()))
	{
		const String errorString = conn_->GetLastErrorString();
		errorCode_ = conn_->GetLastErrorCode();
		if (errorString.Length() > 2)
			_LogError_("MySQL 오류 : %s", conn_->GetLastErrorString().Source());

		return false;
	}

	sqlResult_ = mysql_store_result(conn_->GetConnection());

	if (sqlResult_ == nullptr)
	{
		const String errorString = conn_->GetLastErrorString();
		errorCode_ = conn_->GetLastErrorCode();
		if (errorString.Length() > 2)
			_LogError_("MySQL 오류 : %s", conn_->GetLastErrorString().Source());
		return false;
	}


	MYSQL_FIELD* pField;

	// 필드 이름에서 필드 인덱스를 얻을 수 있도록 하고
	// 필드 인덱스에서 필드 이름을 얻을 수 있도록 한다.
	int fieldIndex = 0;
	while ((pField = mysql_fetch_field(sqlResult_)))
	{
		fieldList_.Insert(pField->name, fieldIndex);
		fieldIndex++;
	}

	sqlRow_ = mysql_fetch_row(sqlResult_);
	return true;
}

bool MysqlQuerySelect::HasNext() const
{
	return sqlRow_;
}

bool MysqlQuerySelect::Next()
{
	sqlRow_ = mysql_fetch_row(sqlResult_);
	return sqlRow_;
}

NS_END