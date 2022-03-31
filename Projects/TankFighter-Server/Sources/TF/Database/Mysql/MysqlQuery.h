/*
	»ý¼ºÀÏ : 2019/03/06
 */


#ifndef _DFDATABASE_QUERY_H_
#define _DFDATABASE_QUERY_H_

#include <string>
#include <map>
#include <sstream>

#include <TF/Database/Mysql/MysqlConnection.h>


class MySQLQuery
{

public:

	MySQLQuery(MySQLConnection *mConn, const std::string &sStatement);
	~MySQLQuery();

	// sets the value of idx to a given string (also adds quotation marks and escapes the string)
	bool setString(const unsigned int &idx, const std::string &value);
	// sets the value of idx to a given int
	bool setInt(const unsigned int &idx, const int &value);
	// sets the value of idx to a given double
	bool setDouble(const unsigned int &idx, const double &value);
	// sets the value of idx to a given time_t
	bool setTime(const unsigned int &idx, const time_t &value);
	// sets the value of idx to NULL
	bool setNull(const unsigned int &idx);

	// executes a SELECT-statement
	bool ExecuteQuery();
	// executes an UPDATE-statement
	bool ExecuteUpdate();
	// executes an INSERT-statement and returns the last inserted ID
	int ExecuteInsert();

	// builds the query string with filled-in arguments and returns it
	const std::string BuildQueryString();

	// returns a field name
	const std::string getFieldName(const unsigned int &field);
	// gets a string value from the given row and field
	const std::string getString(const unsigned int &row, const unsigned int &field);
	const std::string getString(const unsigned int &row, const std::string &field);
	// gets an int value from the given row and field
	int getInt(const unsigned int &row, const unsigned int &field);
	int getInt(const unsigned int &row, const std::string &field);
	// gets a double value from the given row and field
	double getDouble(const unsigned int &row, const unsigned int &field);
	double getDouble(const unsigned int &row, const std::string &field);
	// gets a time value from the given row and field
	time_t getTime(const unsigned int &row, const unsigned int &field);
	time_t getTime(const unsigned int &row, const std::string &field);

	// returns the result row count
	unsigned int GetResultRowCount();
	unsigned int GetFieldCount();


private:
	typedef std::map<int, std::string> TResultRow;
	MySQLConnection *m_sqlConn;
	int m_iResultRowCount;
	std::string m_sStatement;
	std::map<int, std::string> m_mArgMap;
	std::map<int, TResultRow> m_mResultMap;
	std::map<int, std::string> m_mFieldMap;
	std::map<std::string, int> m_mFieldStringToIntMap;

};
#endif