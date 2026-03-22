#pragma once

#include "jdb/IConnection.h"
#include "mysql.h"

NS_JDB_BEGIN

class JDB_DLL MysqlConnection : public IConnection
{
public:
	MysqlConnection();
	~MysqlConnection() override;

	bool Connect(const jc::String& _hostname, const uint16_t& _port, const jc::String& _username,
	             const jc::String& _password, const jc::String& _dbName = "") override;
	bool SelectDB(const jc::String& _dbName) override;
	void Disconnect() override;

	virtual jc::String GetLastErrorString() const override;
	virtual int GetLastErrorCode() const override;
	virtual int GetLastError(OUT jc::String& _str) const override;
	virtual jc::String EscapeString(const jc::String& _value) const override;

	MYSQL* GetConnection() const;

private:
	MYSQL* mySqlConn_;
};

NS_END