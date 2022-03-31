#include <TF/Database/Mysql/MysqlQuery.h>
#include <TF/Util/Console.h>



MySQLQuery::MySQLQuery(MySQLConnection *mConn, const std::string &sStatement)
{
	m_sqlConn = mConn;
	m_sStatement = sStatement;
	m_iResultRowCount = 0;

	int argCount = std::count(m_sStatement.begin(), m_sStatement.end(), '?');
	for (int i = 1; i <= argCount; i++)
	{
		m_mArgMap.insert(std::pair<int, std::string>(i, ""));
	}
}

MySQLQuery::~MySQLQuery()
{
}

bool MySQLQuery::setString(const unsigned int &idx, const std::string &value)
{

	if (idx > m_mArgMap.size())
	{
		Console::ErrorWriteLine("Index exceeds total arg count in statement");
		return false;
	}

	std::stringstream ss;
	std::string escapedValue = m_sqlConn->EscapeString(value);
	ss << "\"" << escapedValue << "\"";
	m_mArgMap[idx] = ss.str();

	return true;
}

bool MySQLQuery::setInt(const unsigned int &idx, const int &value)
{
	if (idx > m_mArgMap.size())
	{
		Console::ErrorWriteLine("Index exceeds total arg count in statement");
		return false;
	}

	std::stringstream ss;
	ss << value;
	m_mArgMap[idx] = ss.str();

	return true;
}

bool MySQLQuery::setDouble(const unsigned int &idx, const double &value)
{
	if (idx > m_mArgMap.size())
	{
		Console::ErrorWriteLine("Index exceeds total arg count in statement");
		return false;
	}

	std::stringstream ss;
	ss << value;
	m_mArgMap[idx] = ss.str();

	return true;
}

bool MySQLQuery::setTime(const unsigned int &idx, const time_t &value)
{
	if (idx > m_mArgMap.size())
	{
		Console::ErrorWriteLine("Index exceeds total arg count in statement");
		return false;
	}

	std::stringstream ss;
	ss << value;
	m_mArgMap[idx] = ss.str();


	return true;
}

bool MySQLQuery::setNull(const unsigned int &idx)
{
	if (idx > m_mArgMap.size())
	{
		Console::ErrorWriteLine("Index exceeds total arg count in statement");
		return false;
	}

	m_mArgMap[idx] = "NULL";
	return true;
}

const std::string MySQLQuery::getFieldName(const unsigned int &field)
{
	if (field < 1)
	{
		Console::ErrorWriteLine("The field index has to be over 1!");
		return NULL;
	}
	else if (m_mFieldMap.size() < field) {
		Console::ErrorWriteLine("There are only %d fields available!", m_mFieldMap.size());
		return NULL;
	}

	std::string sFieldName = m_mFieldMap[field];
	return sFieldName;
}

const std::string MySQLQuery::getString(const unsigned int &row, const unsigned int &field)
{
	if (GetResultRowCount() < 1)
	{
		Console::ErrorWriteLine("The query didn't return any rows!");
		return NULL;
	}
	else if (GetResultRowCount() < row)
	{
		Console::ErrorWriteLine("There are only %d rows available!", GetResultRowCount());
		return NULL;
	}
	else if (row < 1)
	{
		Console::ErrorWriteLine("The selected row has to be > 1");
		return NULL;
	}

	TResultRow rSelectedRow;
	rSelectedRow = m_mResultMap[row - 1];

	std::string sValue = rSelectedRow[field];

	return sValue;
}

const std::string MySQLQuery::getString(const unsigned int &row, const std::string &field)
{
	if (GetResultRowCount() < 1)
	{
		Console::ErrorWriteLine("The query didn't return any rows!");
		return NULL;
	}
	else if (GetResultRowCount() < row)
	{
		Console::ErrorWriteLine("There are only %d rows available!", GetResultRowCount());
		return NULL;
	}
	else if (row < 1)
	{
		Console::ErrorWriteLine("The selected row has to be > 1");
		return NULL;
	}

	TResultRow rSelectedRow;
	rSelectedRow = m_mResultMap[row - 1];

	int iFieldID = m_mFieldStringToIntMap[field];
	std::string sValue = rSelectedRow[iFieldID];

	return sValue;
}

int MySQLQuery::getInt(const unsigned int &row, const unsigned int &field)
{
	if (GetResultRowCount() < 1)
	{
		Console::ErrorWriteLine("The query didn't return any rows!");
		return NULL;
	}
	else if (GetResultRowCount() < row)
	{
		Console::ErrorWriteLine("There are only %d rows available!", GetResultRowCount());
		return NULL;
	}
	else if (row < 1)
	{
		Console::ErrorWriteLine("The selected row has to be > 1");
		return NULL;
	}

	TResultRow rSelectedRow;
	rSelectedRow = m_mResultMap[row - 1];

	int iValue = atoi(rSelectedRow[field].c_str());

	return iValue;
}

int MySQLQuery::getInt(const unsigned int &row, const std::string &field)
{
	if (GetResultRowCount() < 1)
	{
		Console::ErrorWriteLine("The query didn't return any rows!");
		return NULL;
	}
	else if (GetResultRowCount() < row)
	{
		Console::ErrorWriteLine("There are only %d rows available!", GetResultRowCount());
		return NULL;
	}
	else if (row < 1)
	{
		Console::ErrorWriteLine("The selected row has to be > 1");
		return NULL;
	}

	TResultRow rSelectedRow;
	rSelectedRow = m_mResultMap[row - 1];

	int iFieldID = m_mFieldStringToIntMap[field];
	int iValue = atoi(rSelectedRow[iFieldID].c_str());
	return iValue;
}

double MySQLQuery::getDouble(const unsigned int &row, const unsigned int &field)
{
	if (GetResultRowCount() < 1)
	{
		Console::ErrorWriteLine("The query didn't return any rows!");
		return NULL;
	}
	else if (GetResultRowCount() < row)
	{
		Console::ErrorWriteLine("There are only %d rows available!", GetResultRowCount());
		return NULL;
	}
	else if (row < 1)
	{
		Console::ErrorWriteLine("The selected row has to be > 1");
		return NULL;
	}

	TResultRow rSelectedRow;
	rSelectedRow = m_mResultMap[row - 1];

	double dValue = atof(rSelectedRow[field].c_str());

	return dValue;
}

double MySQLQuery::getDouble(const unsigned int &row, const std::string &field)
{
	if (GetResultRowCount() < 1)
	{
		Console::ErrorWriteLine("The query didn't return any rows!");
		return NULL;
	}
	else if (GetResultRowCount() < row)
	{
		Console::ErrorWriteLine("There are only %d rows available!", GetResultRowCount());
		return NULL;
	}
	else if (row < 1)
	{
		Console::ErrorWriteLine("The selected row has to be > 1");
		return NULL;
	}

	TResultRow rSelectedRow;
	rSelectedRow = m_mResultMap[row - 1];

	int iFieldID = m_mFieldStringToIntMap[field];
	double dValue = atof(rSelectedRow[iFieldID].c_str());

	return dValue;
}

time_t MySQLQuery::getTime(const unsigned int &row, const unsigned int &field)
{
	if (GetResultRowCount() < 1)
	{
		Console::ErrorWriteLine("The query didn't return any rows!");
		return NULL;
	}
	else if (GetResultRowCount() < row)
	{
		Console::ErrorWriteLine("There are only %d rows available!", GetResultRowCount());
		return NULL;
	}
	else if (row < 1)
	{
		Console::ErrorWriteLine("The selected row has to be > 1");
		return NULL;
	}

	TResultRow rSelectedRow;
	rSelectedRow = m_mResultMap[row - 1];

	time_t tValue = atoi(rSelectedRow[field].c_str());

	return tValue;
}

time_t MySQLQuery::getTime(const unsigned int &row, const std::string &field)
{
	if (GetResultRowCount() < 1)
	{
		Console::ErrorWriteLine("The query didn't return any rows!");
		return NULL;
	}
	else if (GetResultRowCount() < row)
	{
		Console::ErrorWriteLine("There are only %d rows available!", GetResultRowCount());
		return NULL;
	}
	else if (row < 1)
	{
		Console::ErrorWriteLine("The selected row has to be > 1");
		return NULL;
	}

	TResultRow rSelectedRow;
	rSelectedRow = m_mResultMap[row - 1];

	int iFieldID = m_mFieldStringToIntMap[field];
	time_t tValue = atoi(rSelectedRow[iFieldID].c_str());

	return tValue;
}


unsigned int MySQLQuery::GetResultRowCount()
{
	const int iRowCount = m_mResultMap.size();
	return iRowCount;
}

unsigned int MySQLQuery::GetFieldCount()
{
	const int iFieldCount = m_mFieldMap.size();
	return iFieldCount;
}

const std::string MySQLQuery::BuildQueryString()
{
	// replace each '?' with the corresponding value
	int iLastFoundPos = 0;
	std::string sPreparedStatement;
	sPreparedStatement = m_sStatement;
	for (unsigned int i = 1; i <= m_mArgMap.size(); i++)
	{
		iLastFoundPos = sPreparedStatement.find('?');
		sPreparedStatement.replace(iLastFoundPos, 1, "");
		sPreparedStatement.insert(iLastFoundPos, m_mArgMap[i]);
	}

	return sPreparedStatement;
}

bool MySQLQuery::ExecuteQuery()
{
	std::string sStatement = BuildQueryString();

	if (mysql_query(m_sqlConn->getConn(), sStatement.c_str()))
	{
		std::string erstr = m_sqlConn->GetLastError();
		if (erstr.length() > 2)
			Console::ErrorWriteLine("MySQL Error: %s", m_sqlConn->GetLastError().c_str());
		
		return false;
	}

	MYSQL_RES *result = mysql_store_result(m_sqlConn->getConn());

	if (result == NULL)
	{
		std::string erstr = m_sqlConn->GetLastError();
		if (erstr.length() > 2)
			Console::ErrorWriteLine("MySQL Error: %s", m_sqlConn->GetLastError().c_str());
		return false;
	}


	int iNumFields = mysql_num_fields(result);
	MYSQL_ROW row;
	MYSQL_FIELD *field;

	// Get field names and store it in the map
	int i = 0;
	while ((field = mysql_fetch_field(result)))
	{
		m_mFieldMap.insert(std::pair<int, std::string>(i, field->name));
		m_mFieldStringToIntMap.insert(std::pair<std::string, int>(field->name, i));
		i++;
	}

	// Get Rows
	i = 0;
	while ((row = mysql_fetch_row(result)))
	{
		TResultRow resRow;
		for (int n = 0; n < iNumFields; n++)
		{
			resRow.insert(std::pair<int, std::string>(n, row[n] ? row[n] : "NULL"));
		}

		m_mResultMap.insert(std::pair<int, TResultRow>(i, resRow));

		i++;
	}

	return true;
}

bool MySQLQuery::ExecuteUpdate()
{
	std::string sStatement = BuildQueryString();

	if (mysql_query(m_sqlConn->getConn(), sStatement.c_str()))
	{
		std::string erstr = m_sqlConn->GetLastError();
		if (erstr.length() > 2)
			Console::ErrorWriteLine("MySQL Error: %s", m_sqlConn->GetLastError().c_str());
		return false;
	}

	return true;
}

int MySQLQuery::ExecuteInsert()
{
	std::string sStatement = BuildQueryString();

	if (mysql_query(m_sqlConn->getConn(), sStatement.c_str()))
	{
		std::string erstr = m_sqlConn->GetLastError();
		if (erstr.length() > 2)
			Console::ErrorWriteLine("MySQL Error: %s", m_sqlConn->GetLastError().c_str());
		return 0;
	}

	int iLastInsertID = mysql_insert_id(m_sqlConn->getConn());

	return iLastInsertID;
}

