#pragma once

#include "jdb/IConnection.h"

NS_JDB_BEGIN

class JDB_DLL SqlServerConnection : public IConnection
{
public:
	SqlServerConnection();
	~SqlServerConnection() override;

	bool Connect(const jc::String& _hostname, const uint16_t& _port, const jc::String& _username,
	             const jc::String& _password, const jc::String& _dbName = "") override;
	bool SelectDB(const jc::String& _dbName) override;
	void Disconnect() override;

	virtual jc::String GetLastErrorString() const override;
	virtual int GetLastErrorCode() const override;
	virtual	int GetLastError(OUT jc::String& _str) const override;
	jc::String EscapeString(const jc::String& _value) const override;

	SQLHDBC GetConnection() const;
	SQLHENV GetEnvironment() const;

	// 트랜잭션 제어
	bool BeginTransaction();
	bool Commit();
	bool Rollback();
	bool SetAutoCommit(bool _enable);

private:
	jc::String GetDiagnosticMessage(SQLSMALLINT _handleType, SQLHANDLE _handle) const;
	SQLINTEGER GetDiagnosticCode(SQLSMALLINT _handleType, SQLHANDLE _handle) const;

	int			GetDiagnostic(SQLSMALLINT _handleType, SQLHANDLE _handle, OUT jc::String& _msg) const;

private:
	SQLHENV hEnv_;
	SQLHDBC hDbc_;
};

NS_END
