#include "Server.h"
#include "MysqlQuery.h"

USING_NS_JC;
USING_NS_JC;
USING_NS_STD;

MysqlQuery::MysqlQuery(MysqlConnection* conn, const String& preparedStatement, StatementType type)
	: m_eType(type),
	  m_pConn(conn),
	  m_PreparedStatement(preparedStatement),
	  m_bSuccess(false) {}

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



int MysqlQuerySelect::GetFieldIndex(const char* fieldName) {
	const int* pFind = m_hFieldList.Find(fieldName);

	if (pFind == nullptr) {
		return InvalidValue_v;
	}

	return *pFind;
}

// 필드 값이 존재하지 않는 경우 nullptr이 반환될 수 있음
const char* MysqlQuerySelect::GetRawString(const char* fieldName) {
	if (!IsSuccess()) {
		_LogError_("쿼리 수행결과가 존재하지 않습니다. %s", "GetRawString()");
		return nullptr;
	}

	const int iFieldIndex = GetFieldIndex(fieldName);

	if (iFieldIndex == InvalidValue_v) {
		_LogError_("%s 필드를 찾지 못했습니다. %s", fieldName, "GetRawString()");
		return nullptr;
	}

	return m_SqlRow[iFieldIndex];
}


String MysqlQuerySelect::GetString(const char* fieldName) {
	return GetRawString(fieldName);
}

DateTime MysqlQuerySelect::GetDateTime(const char* fieldName) {
	const char* pRawString = GetRawString(fieldName);
	if (pRawString == nullptr) return 0;
	return DateTime::TryParse()
}

Int32U MysqlQuerySelect::GetRowCount() const {

	if (!IsSuccess()) {
		_LogError_("쿼리 수행결과가 존재하지 않습니다. GetFieldCount()");
		return 0;
	}

	return (Int32U)mysql_num_rows(m_SqlResult);
}

Int32U MysqlQuerySelect::GetFieldCount() const {

	if (!IsSuccess()) {
		_LogError_("쿼리 수행결과가 존재하지 않습니다. GetFieldCount()");
		return 0;
	}

	return m_hFieldList.Size();
}


MysqlQuerySelect::~MysqlQuerySelect() {
	if (m_SqlResult) {
		mysql_free_result(m_SqlResult);
		m_SqlResult = nullptr;
	}
}

bool MysqlQuerySelect::Execute() {
	if (mysql_query(m_pConn->GetConnection(), m_PreparedStatement.Source())) {
		const String erstr = m_pConn->GetLastError();
		if (erstr.Length() > 2)
			_LogError_("MySQL 오류 : %s", m_pConn->GetLastError().Source());

		return false;
	}

	m_SqlResult = mysql_store_result(m_pConn->GetConnection());

	if (m_SqlResult == nullptr) {
		const String erstr = m_pConn->GetLastError();
		if (erstr.Length() > 2)
			_LogError_("MySQL 오류 : %s", m_pConn->GetLastError().Source());
		return false;
	}


	MYSQL_FIELD* field;

	// 필드 이름에서 필드 인덱스를 얻을 수 있도록 하고
	// 필드 인덱스에서 필드 이름을 얻을 수 있도록 한다.
	int i = 0;
	while ((field = mysql_fetch_field(m_SqlResult))) {
		m_hFieldList.Insert(field->name, i);
		i++;
	}

	m_SqlRow = mysql_fetch_row(m_SqlResult);
	return m_bSuccess = true;
}

bool MysqlQuerySelect::HasNext() const {
	return m_SqlRow;
}

bool MysqlQuerySelect::Next() {
	m_SqlRow = mysql_fetch_row(m_SqlResult);
	return m_SqlRow;
}
