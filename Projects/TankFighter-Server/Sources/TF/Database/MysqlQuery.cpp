#include <TF/Database/MysqlQuery.h>
#include <TF/Util/Console.h>


MysqlQuery::MysqlQuery(MysqlConnection *mConn, const JCore::String& preparedStatement)
{
	m_sqlConn = mConn;
	m_PreparedStatement = preparedStatement;
	m_bSuccess = false;
}

MysqlQuery::~MysqlQuery()
{
}

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


const JCore::String MysqlQuery::GetFieldName(const unsigned int &field)
{
	if (field < 1)
	{
		Console::WriteLine(ConsoleColor::RED, "The field index has to be over 1!");
		return NULL;
	}
	else if (m_FieldMap.size() < field) {
		Console::WriteLine(ConsoleColor::RED, "There are only %d fields available!", m_FieldMap.size());
		return NULL;
	}

	JCore::String sFieldName = m_FieldMap[field];
	return sFieldName;
}

const JCore::String MysqlQuery::GetString(const unsigned int &row, const unsigned int &field)
{
	if (GetResultRowCount() < 1)
	{
		Console::WriteLine(ConsoleColor::RED, "The query didn't return any rows!");
		return NULL;
	}
	else if (GetResultRowCount() < row)
	{
		Console::WriteLine(ConsoleColor::RED, "There are only %d rows available!", GetResultRowCount());
		return NULL;
	}
	else if (row < 1)
	{
		Console::WriteLine(ConsoleColor::RED, "The selected row has to be > 1");
		return NULL;
	}

	TResultRow rSelectedRow = m_ResultMap[row - 1];

	JCore::String sValue = rSelectedRow[field];

	return sValue;
}

const JCore::String MysqlQuery::GetString(const unsigned int &row, const JCore::String &field)
{
	if (GetResultRowCount() < 1)
	{
		Console::WriteLine(ConsoleColor::RED, "The query didn't return any rows!");
		return NULL;
	}
	else if (GetResultRowCount() < row)
	{
		Console::WriteLine(ConsoleColor::RED, "There are only %d rows available!", GetResultRowCount());
		return NULL;
	}
	else if (row < 1)
	{
		Console::WriteLine(ConsoleColor::RED, "The selected row has to be > 1");
		return NULL;
	}

	TResultRow rSelectedRow = m_ResultMap[row - 1];

	const int iFieldID = m_FieldStringToIntMap[field];
	JCore::String sValue = rSelectedRow[iFieldID];

	return sValue;
}

int MysqlQuery::GetInt(const unsigned int &row, const unsigned int &field)
{
	if (GetResultRowCount() < 1)
	{
		Console::WriteLine(ConsoleColor::RED, "The query didn't return any rows!");
		return NULL;
	}
	else if (GetResultRowCount() < row)
	{
		Console::WriteLine(ConsoleColor::RED, "There are only %d rows available!", GetResultRowCount());
		return NULL;
	}
	else if (row < 1)
	{
		Console::WriteLine(ConsoleColor::RED, "The selected row has to be > 1");
		return NULL;
	}

	TResultRow rSelectedRow = m_ResultMap[row - 1];
	const int iValue = atoi(rSelectedRow[field].Source());

	return iValue;
}

int MysqlQuery::GetInt(const unsigned int &row, const JCore::String &field)
{
	if (GetResultRowCount() < 1)
	{
		Console::WriteLine(ConsoleColor::RED, "The query didn't return any rows!");
		return NULL;
	}
	else if (GetResultRowCount() < row)
	{
		Console::WriteLine(ConsoleColor::RED, "There are only %d rows available!", GetResultRowCount());
		return NULL;
	}
	else if (row < 1)
	{
		Console::WriteLine(ConsoleColor::RED, "The selected row has to be > 1");
		return NULL;
	}

	TResultRow rSelectedRow = m_ResultMap[row - 1];

	const int iFieldID = m_FieldStringToIntMap[field];
	const int iValue = atoi(rSelectedRow[iFieldID].Source());
	return iValue;
}

double MysqlQuery::GetDouble(const unsigned int &row, const unsigned int &field)
{
	if (GetResultRowCount() < 1)
	{
		Console::WriteLine(ConsoleColor::RED, "The query didn't return any rows!");
		return NULL;
	}
	else if (GetResultRowCount() < row)
	{
		Console::WriteLine(ConsoleColor::RED, "There are only %d rows available!", GetResultRowCount());
		return NULL;
	}
	else if (row < 1)
	{
		Console::WriteLine(ConsoleColor::RED, "The selected row has to be > 1");
		return NULL;
	}

	TResultRow rSelectedRow = m_ResultMap[row - 1];

	const double dValue = atof(rSelectedRow[field].Source());

	return dValue;
}

double MysqlQuery::GetDouble(const unsigned int &row, const JCore::String &field)
{
	if (GetResultRowCount() < 1)
	{
		Console::WriteLine(ConsoleColor::RED, "The query didn't return any rows!");
		return NULL;
	}
	else if (GetResultRowCount() < row)
	{
		Console::WriteLine(ConsoleColor::RED, "There are only %d rows available!", GetResultRowCount());
		return NULL;
	}
	else if (row < 1)
	{
		Console::WriteLine(ConsoleColor::RED, "The selected row has to be > 1");
		return NULL;
	}

	TResultRow rSelectedRow = m_ResultMap[row - 1];

	const int iFieldID = m_FieldStringToIntMap[field];
	const double dValue = atof(rSelectedRow[iFieldID].Source());

	return dValue;
}

JCore::DateTime MysqlQuery::GetTime(const unsigned int &row, const unsigned int &field)
{
	if (GetResultRowCount() < 1)
	{
		Console::WriteLine(ConsoleColor::RED, "The query didn't return any rows!");
		return NULL;
	}
	else if (GetResultRowCount() < row)
	{
		Console::WriteLine(ConsoleColor::RED, "There are only %d rows available!", GetResultRowCount());
		return NULL;
	}
	else if (row < 1)
	{
		Console::WriteLine(ConsoleColor::RED, "The selected row has to be > 1");
		return NULL;
	}

	TResultRow rSelectedRow = m_ResultMap[row - 1];
	JCore::String val = rSelectedRow[field];

	int precision = atoi(val.GetRange(20, 5).Source());
	int mili = precision / 1000;
	precision -= precision * mili;
	int micro = precision;

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

JCore::DateTime MysqlQuery::GetTime(const unsigned int &row, const JCore::String &field)
{
	if (GetResultRowCount() < 1)
	{
		Console::WriteLine(ConsoleColor::RED, "The query didn't return any rows!");
		return NULL;
	}
	else if (GetResultRowCount() < row)
	{
		Console::WriteLine(ConsoleColor::RED, "There are only %d rows available!", GetResultRowCount());
		return NULL;
	}
	else if (row < 1)
	{
		Console::WriteLine(ConsoleColor::RED, "The selected row has to be > 1");
		return NULL;
	}

	TResultRow rSelectedRow = m_ResultMap[row - 1];

	const int iFieldID = m_FieldStringToIntMap[field];
	JCore::String val = rSelectedRow[iFieldID];
	int precision = atoi(val.GetRange(20, 25).Source());
	int mili = precision / 1000;
	precision -= precision * mili;
	int micro = precision;

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


unsigned int MysqlQuery::GetResultRowCount()
{
	const int iRowCount = m_ResultMap.size();
	return iRowCount;
}

unsigned int MysqlQuery::GetFieldCount()
{
	const int iFieldCount = m_FieldMap.size();
	return iFieldCount;
}

MysqlQueryResult MysqlQuery::Result() {
	return { this, m_bSuccess };
}



bool MysqlQuery::ExecuteQuery()
{
	if (mysql_query(m_sqlConn->getConn(), m_PreparedStatement.Source()))
	{
		const JCore::String erstr = m_sqlConn->GetLastError();
		if (erstr.Length() > 2)
			Console::WriteLine(ConsoleColor::RED, "MySQL Error: %s", m_sqlConn->GetLastError().Source());
		
		return false;
	}

	MYSQL_RES *result = mysql_store_result(m_sqlConn->getConn());

	if (result == NULL)
	{
		const JCore::String erstr = m_sqlConn->GetLastError();
		if (erstr.Length() > 2)
			Console::WriteLine(ConsoleColor::RED, "MySQL Error: %s", m_sqlConn->GetLastError().Source());
		return false;
	}


	const int iNumFields = mysql_num_fields(result);
	MYSQL_ROW row;
	MYSQL_FIELD *field;

	// Get field names and store it in the map
	int i = 0;
	while ((field = mysql_fetch_field(result)))
	{
		m_FieldMap.insert(std::pair<int, JCore::String>(i, field->name));
		m_FieldStringToIntMap.insert(std::pair<JCore::String, int>(field->name, i));
		i++;
	}

	// Get Rows
	i = 0;
	while ((row = mysql_fetch_row(result)))
	{
		TResultRow resRow;
		for (int n = 0; n < iNumFields; n++)
			resRow.insert(std::pair<int, JCore::String>(n, row[n] ? row[n] : "NULL"));

		m_ResultMap.insert(std::pair<int, TResultRow>(i, resRow));

		i++;
	}

	
	return m_bSuccess = true;
}

bool MysqlQuery::ExecuteUpdate()
{
	if (mysql_query(m_sqlConn->getConn(), m_PreparedStatement.Source()))
	{
		const JCore::String erstr = m_sqlConn->GetLastError();
		if (erstr.Length() > 2)
			Console::WriteLine(ConsoleColor::RED, "MySQL Error: %s", m_sqlConn->GetLastError().Source());
		return false;
	}

	return m_bSuccess = true;
}

int MysqlQuery::ExecuteInsert()
{
	if (mysql_query(m_sqlConn->getConn(), m_PreparedStatement.Source()))
	{
		const JCore::String erstr = m_sqlConn->GetLastError();
		if (erstr.Length() > 2)
			Console::WriteLine(ConsoleColor::RED, "MySQL Error: %s", m_sqlConn->GetLastError().Source());
		return 0;
	}

	const int iLastInsertID = mysql_insert_id(m_sqlConn->getConn());

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

