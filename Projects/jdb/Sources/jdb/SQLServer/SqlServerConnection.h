#pragma once

#include <jc/Primitives/String.h>

NS_JDB_BEGIN

class JDB_DLL SqlServerConnection
{
public:
	SqlServerConnection();
	~SqlServerConnection();

	// SQL Server 접속 (ODBC)
	bool Connect(const jc::String& _hostname, const uint16_t& _port, const jc::String& _username,
	             const jc::String& _password, const jc::String& _dbName = "");

	// DB 선택
	bool SelectDB(const jc::String& _dbName);

	// 연결 끊기
	void Disconnect();

	// 제일 마지막 오류
	jc::String GetLastErrorString() const;
	int GetLastErrorCode() const;

	SQLHDBC GetConnection() const;
	SQLHENV GetEnvironment() const;
	bool IsConnected() const;

	// 이스케이프 스트링 반환 (SQL Server: 싱글 쿼트 더블링)
	jc::String EscapeString(const jc::String& _value) const;

	// 트랜잭션 제어
	bool BeginTransaction();
	bool Commit();
	bool Rollback();
	bool SetAutoCommit(bool _enable);

private:
	// ODBC 진단 정보 추출
	jc::String GetDiagnosticMessage(SQLSMALLINT _handleType, SQLHANDLE _handle) const;
	SQLINTEGER GetDiagnosticCode(SQLSMALLINT _handleType, SQLHANDLE _handle) const;

private:
	SQLHENV hEnv_;
	SQLHDBC hDbc_;
	bool isConnected_;
	jc::String hostname_ = "";
	jc::String username_ = "";
	jc::String password_ = "";
	jc::String dbName_ = "";
	uint16_t port_;
};

NS_END
