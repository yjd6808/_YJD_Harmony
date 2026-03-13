/*
	생성일 : 2026/03/07
	작성자 : 윤정도
	*/


#pragma once


#include <jc/Container/LinkedList.h>
#include <jc/Sync/NormalLock.h>
#include <jc/Primitives/String.h>

NS_JDB_BEGIN

class SqlServerConnection;

class JDB_DLL SqlServerConnectionPool
{
public:
	SqlServerConnection* GetConnection();
	void ReleaseConnection(SqlServerConnection* _pConnection);
	~SqlServerConnectionPool();

	bool Init(const uint32_t _initConn);

	static constexpr const char* TypeName()
	{
		return "SqlServer 커넥션 풀";
	}

private:
	void TerminateAllConnections();
	static void TerminateConnection(SqlServerConnection* _pConnection);
	SqlServerConnection* CreateConnection() const;

private:
	SqlServerConnectionPool(
		const jc::String& _hostName,
		_u16 _port,
		const jc::String& _id,
		const jc::String& _pass,
		const jc::String& _dbName,
		int _maxConn
	);

	jc::String hostName_;
	jc::String accountId_;
	jc::String accountPass_;
	jc::String dbName_;
	_u16 port_;
	_u32 maxConnection_;

	int currentConnectionSize_;
	jc::NormalLock mutex_;
	jc::LinkedList<SqlServerConnection*> connectionList_;

	friend class SqlServerDatabase;
};

// 소멸자 호출시 컨넥션 풀로 다시 되돌림
struct SqlServerAutoReleaseConnection
{
	SqlServerAutoReleaseConnection(SqlServerConnection* _pConnection, SqlServerConnectionPool* _pConnectionPool)
	: connection_(_pConnection)
	, connectionPool_(_pConnectionPool)
	{
	}

	~SqlServerAutoReleaseConnection()
	{
		jc_assert_msg(connection_ != nullptr, "SqlServerAutoReleaseConn 소멸자 오류 발생 / 컨넥션이 NULL입니다.");
		jc_assert_msg(connectionPool_ != nullptr, "SqlServerAutoReleaseConn 소멸자 오류 발생 / 풀이 NULL입니다.");
		connectionPool_->ReleaseConnection(connection_);
	}

private:
	SqlServerConnection* connection_;
	SqlServerConnectionPool* connectionPool_;
};

NS_END
