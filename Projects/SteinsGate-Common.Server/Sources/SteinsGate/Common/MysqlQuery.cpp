#include "Server.h"
#include "MysqlQuery.h"

USING_NS_JC;
USING_NS_JC;
USING_NS_STD;

MysqlQuery::MysqlQuery(MysqlConnection* conn, const String& preparedStatement, StatementType type)
	: m_pConn(conn)
	, m_PreparedStatement(preparedStatement)
	, m_bSuccess(false)
	, m_eType(type)
{}

StatementType MysqlQuery::ParseStatement(const String& statement) {
	StatementType eStatement = StatementType::None;
	const String statementPrefix = statement.GetRange(0, StatementPrefixLength - 1).ToLowerCase();

	// 각 스테이트먼트가 각 문자열로 시작하는지 확인
	if (statementPrefix.Find("select") == 0) {
		eStatement = StatementType::Select;
	} else if (statementPrefix.Find("update") == 0) {
		eStatement = StatementType::Update;
	} else if (statementPrefix.Find("insert") == 0) {
		eStatement = StatementType::Insert;
	} else if (statementPrefix.Find("delete") == 0) {
		eStatement = StatementType::Delete;
	}

	return eStatement;
}


bool MysqlQueryUpdate::Execute() {
	if (mysql_query(m_pConn->GetConnection(), m_PreparedStatement.Source())) {
		const String erstr = m_pConn->GetLastError();
		if (erstr.Length() > 2)
			_LogError_("MySQL 오류 : %s", m_pConn->GetLastError().Source());
		return false;
	}

	return m_bSuccess = true;
}

bool MysqlQueryInsert::Execute() {
	if (mysql_query(m_pConn->GetConnection(), m_PreparedStatement.Source())) {
		const String erstr = m_pConn->GetLastError();
		if (erstr.Length() > 2)
			_LogError_("MySQL 오류 : %s", m_pConn->GetLastError().Source());
		return false;
	}

	// https://dev.mysql.com/doc/c-api/5.7/en/mysql-insert-id.html
	m_uiInsertId = mysql_insert_id(m_pConn->GetConnection());
	return true;
}



String MysqlQuerySelect::GetFieldName(const unsigned int& field) {
	if (m_hFieldMap.Size() < field) {
		_LogError_("%d개의 필드밖에 없습니다.", m_hFieldMap.Size());
		return NULL;
	}

	return m_hFieldMap[field];
}

String MysqlQuerySelect::GetString(const unsigned int& rowIdx, const unsigned int& fieldIdx) {
	if (GetResultRowCount() < 1) {
		_LogError_("쿼리 수행결과가 존재하지 않습니다");
		return NULL;
	}

	if (GetResultRowCount() <= rowIdx) {
		_LogError_("%d개의 쿼리 결과만 존재합니다. 입력하신 행 인덱스값이 %d입니다.", GetResultRowCount(), rowIdx);
		return NULL;
	}

	TResultRow& rSelectedRow = m_hResultMap[rowIdx];
	String sValue = rSelectedRow[fieldIdx];

	return sValue;
}

String MysqlQuerySelect::GetString(const unsigned int& rowIdx, const String& fieldName) {
	if (GetResultRowCount() < 1) {
		_LogError_("쿼리 수행결과가 존재하지 않습니다");
		return NULL;
	}

	if (GetResultRowCount() <= rowIdx) {
		_LogError_("%d개의 쿼리 결과만 존재합니다. 입력하신 행 인덱스값이 %d입니다.", GetResultRowCount(), rowIdx);
		return NULL;
	}

	TResultRow& rSelectedRow = m_hResultMap[rowIdx];
	const int iFieldID = m_hFieldStringToIntMap[fieldName];
	String sValue = rSelectedRow[iFieldID];

	return sValue;
}

int MysqlQuerySelect::GetInt(const unsigned int& rowIdx, const unsigned int& fieldIdx)
{
	if (GetResultRowCount() < 1) {
		_LogError_("쿼리 수행결과가 존재하지 않습니다");
		return NULL;
	}

	if (GetResultRowCount() <= rowIdx) {
		_LogError_("%d개의 쿼리 결과만 존재합니다. 입력하신 행 인덱스값이 %d입니다.", GetResultRowCount(), rowIdx);
		return NULL;
	}

	TResultRow& rSelectedRow = m_hResultMap[rowIdx];
	const int iValue = atoi(rSelectedRow[fieldIdx].Source());

	return iValue;
}

int MysqlQuerySelect::GetInt(const unsigned int& rowIdx, const String& fieldName)
{
	if (GetResultRowCount() < 1) {
		_LogError_("쿼리 수행결과가 존재하지 않습니다");
		return NULL;
	}

	if (GetResultRowCount() <= rowIdx) {
		_LogError_("%d개의 쿼리 결과만 존재합니다. 입력하신 행 인덱스값이 %d입니다.", GetResultRowCount(), rowIdx);
		return NULL;
	}
	TResultRow& rSelectedRow = m_hResultMap[rowIdx];

	const int iFieldID = m_hFieldStringToIntMap[fieldName];
	const int iValue = atoi(rSelectedRow[iFieldID].Source());
	return iValue;
}

double MysqlQuerySelect::GetDouble(const unsigned int& rowIdx, const unsigned int& fieldIdx) {
	if (GetResultRowCount() < 1) {
		_LogError_("쿼리 수행결과가 존재하지 않습니다");
		return 0;
	}

	if (GetResultRowCount() <= rowIdx) {
		_LogError_("%d개의 쿼리 결과만 존재합니다. 입력하신 행 인덱스값이 %d입니다.", GetResultRowCount(), rowIdx);
		return 0;
	}

	TResultRow& rSelectedRow = m_hResultMap[rowIdx];
	const double dValue = atof(rSelectedRow[fieldIdx].Source());

	return dValue;
}

double MysqlQuerySelect::GetDouble(const unsigned int& rowIdx, const String& fieldName) {
	if (GetResultRowCount() < 1) {
		_LogError_("쿼리 수행결과가 존재하지 않습니다");
		return 0;
	}
	if (GetResultRowCount() <= rowIdx) {
		_LogError_("%d개의 쿼리 결과만 존재합니다. 입력하신 행 인덱스값이 %d입니다.", GetResultRowCount(), rowIdx);
		return 0;
	}

	TResultRow& rSelectedRow = m_hResultMap[rowIdx];

	const int iFieldID = m_hFieldStringToIntMap[fieldName];
	const double dValue = atof(rSelectedRow[iFieldID].Source());

	return dValue;
}

DateTime MysqlQuerySelect::GetTime(const unsigned int& rowIdx, const unsigned int& fieldIdx) {
	if (GetResultRowCount() < 1) {
		_LogError_("쿼리 수행결과가 존재하지 않습니다");
		return 0;
	}

	if (GetResultRowCount() <= rowIdx) {
		_LogError_("%d개의 쿼리 결과만 존재합니다. 입력하신 행 인덱스값이 %d입니다.", GetResultRowCount(), rowIdx);
		return 0;
	}

	TResultRow& rSelectedRow = m_hResultMap[rowIdx];
	const String& val = rSelectedRow[fieldIdx];

	int precision = atoi(val.GetRange(20, 5).Source());
	const int mili = precision / 1000;

	precision -= precision * mili;
	const int micro = precision;

	return JCore::DateAndTime(
		atoi(val.GetRange(0, 3).Source()),
		atoi(val.GetRange(5, 6).Source()),
		atoi(val.GetRange(8, 9).Source()),
		atoi(val.GetRange(11, 12).Source()),
		atoi(val.GetRange(14, 15).Source()),
		atoi(val.GetRange(17, 18).Source()),
		mili,
		micro
	).ToDateTime();
}

DateTime MysqlQuerySelect::GetTime(const unsigned int& rowIdx, const String& fieldName) {
	if (GetResultRowCount() < 1) {
		_LogError_("쿼리 수행결과가 존재하지 않습니다");
		return 0;
	}
	if (GetResultRowCount() <= rowIdx) {
		_LogError_("%d개의 쿼리 결과만 존재합니다. 입력하신 행 인덱스값이 %d입니다.", GetResultRowCount(), rowIdx);
		return 0;
	}

	TResultRow& rSelectedRow = m_hResultMap[rowIdx];

	const int iFieldID = m_hFieldStringToIntMap[fieldName];
	const String& val = rSelectedRow[iFieldID];
	int precision = atoi(val.GetRange(20, 25).Source());
	const int mili = precision / 1000;

	precision -= precision * mili;
	const int micro = precision;

	return JCore::DateAndTime(
		atoi(val.GetRange(0, 3).Source()),
		atoi(val.GetRange(5, 6).Source()),
		atoi(val.GetRange(8, 9).Source()),
		atoi(val.GetRange(11, 12).Source()),
		atoi(val.GetRange(14, 15).Source()),
		atoi(val.GetRange(17, 18).Source()),
		mili,
		micro
	).ToDateTime();
}


unsigned int MysqlQuerySelect::GetResultRowCount() const {
	const int iRowCount = m_hResultMap.Size();
	return iRowCount;
}

unsigned int MysqlQuerySelect::GetFieldCount() const {
	const int iFieldCount = m_hFieldMap.Size();
	return iFieldCount;
}


bool MysqlQuerySelect::Execute() {
	if (mysql_query(m_pConn->GetConnection(), m_PreparedStatement.Source())) {
		const String erstr = m_pConn->GetLastError();
		if (erstr.Length() > 2)
			_LogError_("MySQL 오류 : %s", m_pConn->GetLastError().Source());

		return false;
	}

	MYSQL_RES* result = mysql_store_result(m_pConn->GetConnection());

	if (result == NULL) {
		const String erstr = m_pConn->GetLastError();
		if (erstr.Length() > 2)
			_LogError_("MySQL 오류 : %s", m_pConn->GetLastError().Source());
		return false;
	}


	const int iNumFields = mysql_num_fields(result);
	MYSQL_ROW row;
	MYSQL_FIELD* field;

	// 필드 이름에서 필드 인덱스를 얻을 수 있도록 하고
	// 필드 인덱스에서 필드 이름을 얻을 수 있도록 한다.
	int i = 0;
	while ((field = mysql_fetch_field(result))) {
		m_hFieldMap.Insert(i, field->name);
		m_hFieldStringToIntMap.Insert(field->name, i);
		i++;
	}

	// 쿼리 실행 결과로 출력된 행들을 가져온다.
	i = 0;
	while ((row = mysql_fetch_row(result))) {
		TResultRow resRow;
		for (int n = 0; n < iNumFields; n++)
			resRow.Insert(n, row[n] ? row[n] : "NULL");

		m_hResultMap.Insert(i, resRow);

		i++;
	}

	return m_hResultMap.Size();
}