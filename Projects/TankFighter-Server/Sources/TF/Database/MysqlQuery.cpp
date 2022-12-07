#include <TF/PrecompiledHeader.h>
#include <TF/Database/MysqlQuery.h>
#include <JCore/Utils/Console.h>

using namespace JCore;

MysqlQuery::MysqlQuery(MysqlConnection *mConn, const JCore::String& preparedStatement) {
	m_sqlConn = mConn;
	m_PreparedStatement = preparedStatement;
	m_bSuccess = false;
}

MysqlQuery::~MysqlQuery() {}

StatementType MysqlQuery::ParseStatement(const JCore::String& statement) {
	StatementType eStatement = StatementType::None;
	const JCore::String statementPrefix = statement.GetRange(0, 5).ToLowerCase();

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


JCore::String MysqlQuery::GetFieldName(const unsigned int& field) {
	if (m_FieldMap.Size() < field) {
		SafeConsole::WriteLine(ConsoleColor::Red, "%d개의 필드밖에 없습니다.", m_FieldMap.Size());
		return NULL;
	}

	return m_FieldMap[field];
}

JCore::String MysqlQuery::GetString(const unsigned int& rowIdx, const unsigned int& fieldIdx) {
	if (GetResultRowCount() < 1) {
		SafeConsole::WriteLine(ConsoleColor::Red, "쿼리 수행결과가 존재하지 않습니다");
		return NULL;
	}

	if (GetResultRowCount() <= rowIdx) {
		SafeConsole::WriteLine(ConsoleColor::Red, "%d개의 쿼리 결과만 존재합니다. 입력하신 행 인덱스값이 %d입니다.", GetResultRowCount(), rowIdx);
		return NULL;
	}

	TResultRow& rSelectedRow = m_ResultMap[rowIdx];
	JCore::String sValue = rSelectedRow[fieldIdx];

	return sValue;
}

JCore::String MysqlQuery::GetString(const unsigned int& rowIdx, const JCore::String& fieldName) {
	if (GetResultRowCount() < 1) {
		SafeConsole::WriteLine(ConsoleColor::Red, "쿼리 수행결과가 존재하지 않습니다");
		return NULL;
	}

	if (GetResultRowCount() <= rowIdx) {
		SafeConsole::WriteLine(ConsoleColor::Red, "%d개의 쿼리 결과만 존재합니다. 입력하신 행 인덱스값이 %d입니다.", GetResultRowCount(), rowIdx);
		return NULL;
	}

	TResultRow& rSelectedRow = m_ResultMap[rowIdx];
	const int iFieldID = m_FieldStringToIntMap[fieldName];
	JCore::String sValue = rSelectedRow[iFieldID];

	return sValue;
}

int MysqlQuery::GetInt(const unsigned int &rowIdx, const unsigned int &fieldIdx)
{
	if (GetResultRowCount() < 1) {
		SafeConsole::WriteLine(ConsoleColor::Red, "쿼리 수행결과가 존재하지 않습니다");
		return NULL;
	}

	if (GetResultRowCount() <= rowIdx) {
		SafeConsole::WriteLine(ConsoleColor::Red, "%d개의 쿼리 결과만 존재합니다. 입력하신 행 인덱스값이 %d입니다.", GetResultRowCount(), rowIdx);
		return NULL;
	}

	TResultRow& rSelectedRow = m_ResultMap[rowIdx];
	const int iValue = atoi(rSelectedRow[fieldIdx].Source());

	return iValue;
}

int MysqlQuery::GetInt(const unsigned int &rowIdx, const JCore::String &fieldName)
{
	if (GetResultRowCount() < 1) {
		SafeConsole::WriteLine(ConsoleColor::Red, "쿼리 수행결과가 존재하지 않습니다");
		return NULL;
	}

	if (GetResultRowCount() <= rowIdx) {
		SafeConsole::WriteLine(ConsoleColor::Red, "%d개의 쿼리 결과만 존재합니다. 입력하신 행 인덱스값이 %d입니다.", GetResultRowCount(), rowIdx);
		return NULL;
	}
	TResultRow& rSelectedRow = m_ResultMap[rowIdx];

	const int iFieldID = m_FieldStringToIntMap[fieldName];
	const int iValue = atoi(rSelectedRow[iFieldID].Source());
	return iValue;
}

double MysqlQuery::GetDouble(const unsigned int &rowIdx, const unsigned int &fieldIdx) {
	if (GetResultRowCount() < 1) {
		SafeConsole::WriteLine(ConsoleColor::Red, "쿼리 수행결과가 존재하지 않습니다");
		return 0;
	}

	if (GetResultRowCount() <= rowIdx) {
		SafeConsole::WriteLine(ConsoleColor::Red, "%d개의 쿼리 결과만 존재합니다. 입력하신 행 인덱스값이 %d입니다.", GetResultRowCount(), rowIdx);
		return 0;
	}

	TResultRow& rSelectedRow = m_ResultMap[rowIdx];
	const double dValue = atof(rSelectedRow[fieldIdx].Source());

	return dValue;
}

double MysqlQuery::GetDouble(const unsigned int &rowIdx, const JCore::String &fieldName) {
	if (GetResultRowCount() < 1) {
		SafeConsole::WriteLine(ConsoleColor::Red, "쿼리 수행결과가 존재하지 않습니다");
		return 0;
	}
	if (GetResultRowCount() <= rowIdx) {
		SafeConsole::WriteLine(ConsoleColor::Red, "%d개의 쿼리 결과만 존재합니다. 입력하신 행 인덱스값이 %d입니다.", GetResultRowCount(), rowIdx);
		return 0;
	}

	TResultRow& rSelectedRow = m_ResultMap[rowIdx];

	const int iFieldID = m_FieldStringToIntMap[fieldName];
	const double dValue = atof(rSelectedRow[iFieldID].Source());

	return dValue;
}

JCore::DateTime MysqlQuery::GetTime(const unsigned int &rowIdx, const unsigned int &fieldIdx) {
	if (GetResultRowCount() < 1) {
		SafeConsole::WriteLine(ConsoleColor::Red, "쿼리 수행결과가 존재하지 않습니다");
		return 0;
	}

	if (GetResultRowCount() <= rowIdx) {
		SafeConsole::WriteLine(ConsoleColor::Red, "%d개의 쿼리 결과만 존재합니다. 입력하신 행 인덱스값이 %d입니다.", GetResultRowCount(), rowIdx);
		return 0;
	}

	TResultRow& rSelectedRow = m_ResultMap[rowIdx];
	const JCore::String& val = rSelectedRow[fieldIdx];

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

JCore::DateTime MysqlQuery::GetTime(const unsigned int &rowIdx, const JCore::String &fieldName) {
	if (GetResultRowCount() < 1) {
		SafeConsole::WriteLine(ConsoleColor::Red, "쿼리 수행결과가 존재하지 않습니다");
		return 0;
	}
	if (GetResultRowCount() <= rowIdx) {
		SafeConsole::WriteLine(ConsoleColor::Red, "%d개의 쿼리 결과만 존재합니다. 입력하신 행 인덱스값이 %d입니다.", GetResultRowCount(), rowIdx);
		return 0;
	}

	TResultRow& rSelectedRow = m_ResultMap[rowIdx];

	const int iFieldID = m_FieldStringToIntMap[fieldName];
	const JCore::String& val = rSelectedRow[iFieldID];
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


unsigned int MysqlQuery::GetResultRowCount() const {
	const int iRowCount = m_ResultMap.Size();
	return iRowCount;
}

unsigned int MysqlQuery::GetFieldCount() const {
	const int iFieldCount = m_FieldMap.Size();
	return iFieldCount;
}


bool MysqlQuery::ExecuteQuery() {
	if (mysql_query(m_sqlConn->GetConnection(), m_PreparedStatement.Source())) {
		const JCore::String erstr = m_sqlConn->GetLastError();
		if (erstr.Length() > 2)
			SafeConsole::WriteLine(ConsoleColor::Red, "MySQL 오류 : %s", m_sqlConn->GetLastError().Source());
		
		return false;
	}

	MYSQL_RES *result = mysql_store_result(m_sqlConn->GetConnection());

	if (result == NULL) {
		const JCore::String erstr = m_sqlConn->GetLastError();
		if (erstr.Length() > 2)
			SafeConsole::WriteLine(ConsoleColor::Red, "MySQL 오류 : %s", m_sqlConn->GetLastError().Source());
		return false;
	}


	const int iNumFields = mysql_num_fields(result);
	MYSQL_ROW row;
	MYSQL_FIELD *field;

	// 필드 이름에서 필드 인덱스를 얻을 수 있도록 하고
	// 필드 인덱스에서 필드 이름을 얻을 수 있도록 한다.
	int i = 0;
	while ((field = mysql_fetch_field(result))) {
		m_FieldMap.Insert(i, field->name);
		m_FieldStringToIntMap.Insert(field->name, i);
		i++;
	}

	// 쿼리 실행 결과로 출력된 행들을 가져온다.
	i = 0;
	while ((row = mysql_fetch_row(result))) {
		TResultRow resRow;
		for (int n = 0; n < iNumFields; n++)
			resRow.Insert(n, row[n] ? row[n] : "NULL");

		m_ResultMap.Insert(i, resRow);

		i++;
	}

	
	return m_bSuccess = true;
}

bool MysqlQuery::ExecuteUpdate() {
	if (mysql_query(m_sqlConn->GetConnection(), m_PreparedStatement.Source())) {
		const JCore::String erstr = m_sqlConn->GetLastError();
		if (erstr.Length() > 2)
			SafeConsole::WriteLine(ConsoleColor::Red, "MySQL 오류 : %s", m_sqlConn->GetLastError().Source());
		return false;
	}

	return m_bSuccess = true;
}

Int64U MysqlQuery::ExecuteInsert() {
	if (mysql_query(m_sqlConn->GetConnection(), m_PreparedStatement.Source())) {
		const JCore::String erstr = m_sqlConn->GetLastError();
		if (erstr.Length() > 2)
			SafeConsole::WriteLine(ConsoleColor::Red, "MySQL 오류 : %s", m_sqlConn->GetLastError().Source());
		return 0;
	}

	const Int64U iLastInsertID = mysql_insert_id(m_sqlConn->GetConnection());

	m_bSuccess = true;
	return iLastInsertID;
}

bool MysqlQuery::ExecuteAuto() {
	switch (ParseStatement(m_PreparedStatement)) {
	case StatementType::Select:
		return ExecuteQuery();
	case StatementType::Insert:
		return ExecuteInsert() != 0;
	case StatementType::Update:
	case StatementType::Delete:
		return ExecuteUpdate();
	default:
		return ExecuteQuery();
	}
}

