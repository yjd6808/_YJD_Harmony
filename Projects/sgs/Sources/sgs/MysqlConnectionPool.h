/*
	생성일 : 2019/05/17
	수정일 : 2022/04/03
	작성자 : 윤정도
	*/


#pragma once


#include <jc/Container/LinkedList.h>
#include <jc/Sync/NormalLock.h>
#include <jc/Primitives/String.h>

class MysqlConnection;

class MysqlConnectionPool
{
public:
	MysqlConnection* GetConnection();
	void ReleaseConnection(MysqlConnection* _pConnection);
	~MysqlConnectionPool();

	bool Init(const uint32_t _initConn);

	static constexpr const char* TypeName()
	{
		return "Mysql 커넥션 풀";
	}

private:
	void TerminateAllConnections();
	static void TerminateConnection(MysqlConnection* _pConnection);
	MysqlConnection* CreateConnection() const;

private:
	MysqlConnectionPool(
		const jc::String& _hostName,
		Int16U _port,
		const jc::String& _id,
		const jc::String& _pass,
		const jc::String& _schemaName,
		int _maxConn
	);

	jc::String hostName_;
	jc::String accountId_;
	jc::String accountPass_;
	jc::String schemeName_;
	Int16U port_;
	Int32U maxConnection_;

	int currentConnectionSize_;
	jc::NormalLock mutex_;
	jc::LinkedList<MysqlConnection*> connectionList_;

	friend class MysqlDatabase;
};

// 소멸자 호출시 컨넥션 풀로 다시 되돌림
struct AutoReleaseConnection
{
	AutoReleaseConnection(MysqlConnection* _pConnection, MysqlConnectionPool* _pConnectionPool)
	: connection_(_pConnection)
	, connectionPool_(_pConnectionPool)
	{
	}

	~AutoReleaseConnection()
	{
		jc_assert_msg(connection_ != nullptr, "AutoReleaseConn 소멸자 오류 발생 / 컨넥션이 NULL입니다.");
		jc_assert_msg(connectionPool_ != nullptr, "AutoReleaseConn 소멸자 오류 발생 / 풀이 NULL입니다.");
		connectionPool_->ReleaseConnection(connection_);
	}

private:
	MysqlConnection* connection_;
	MysqlConnectionPool* connectionPool_;
};
