/*
	생성일 : 2019/05/17
	수정일 : 2022/04/03
	작성자 : 윤정도
	*/


#pragma once


#include <JCore/Container/LinkedList.h>
#include <JCore/Sync/NormalLock.h>
#include <JCore/Primitives/String.h>

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
		const JCore::String& _hostName,
		Int16U _port,
		const JCore::String& _id,
		const JCore::String& _pass,
		const JCore::String& _schemaName,
		int _maxConn
	);

	JCore::String hostName_;
	JCore::String accountId_;
	JCore::String accountPass_;
	JCore::String schemeName_;
	Int16U port_;
	Int32U maxConnection_;

	int currentConnectionSize_;
	JCore::NormalLock mutex_;
	JCore::LinkedList<MysqlConnection*> connectionList_;

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
		DebugAssertMsg(connection_ != nullptr, "AutoReleaseConn 소멸자 오류 발생 / 컨넥션이 NULL입니다.");
		DebugAssertMsg(connectionPool_ != nullptr, "AutoReleaseConn 소멸자 오류 발생 / 풀이 NULL입니다.");
		connectionPool_->ReleaseConnection(connection_);
	}

private:
	MysqlConnection* connection_;
	MysqlConnectionPool* connectionPool_;
};
