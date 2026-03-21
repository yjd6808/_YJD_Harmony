/*
 * 작성자: 윤정도
 * 생성일: 3/13/2026
 * =====================
 *
 * 통합 커넥션 풀
 * IConnection 기반으로 MySQL/SQLServer 공통으로 사용한다.
 */

#pragma once

#include "jc/Container/LinkedList.h"
#include "jc/Sync/NormalLock.h"
#include "jc/Primitives/String.h"

#include "jdb/Namespace.h"
#include "jdb/DLLExport.h"

NS_JDB_BEGIN

class IConnection;

class JDB_DLL ConnectionPool
{
public:
	using ConnectionFactory = IConnection* (*)();

	ConnectionPool(
		DatabaseType _dbType,
		const jc::String& _hostName,
		_u16 _port,
		const jc::String& _id,
		const jc::String& _pass,
		const jc::String& _dbName,
		int _maxConn);
	~ConnectionPool();

	bool Init(uint32_t _initConn);
	IConnection* GetConnection();
	void ReleaseConnection(IConnection* _pConnection);
	void GetConnCount(OUT int& _usingConnCount, OUT int& _availableConnCount) const;

private:
	void TerminateAllConnections();
	static void TerminateConnection(IConnection* _pConnection);
	IConnection* CreateConnection() const;

private:
	DatabaseType dbType_;
	jc::String hostName_;
	jc::String accountId_;
	jc::String accountPass_;
	jc::String dbName_;
	_u16 port_;
	_u32 maxConnection_;

	int usingConnCount_ = 0;
	mutable jc::NormalLock mutex_;
	jc::LinkedList<IConnection*> connectionList_;
};

// 소멸자 호출시 커넥션 풀로 다시 되돌림
struct AutoReleaseConnection
{
	AutoReleaseConnection(IConnection* _pConnection, ConnectionPool* _pConnectionPool)
	: connection_(_pConnection)
	, connectionPool_(_pConnectionPool)
	{
	}

	~AutoReleaseConnection()
	{
		jc_assert_msg(connection_ != nullptr, "AutoReleaseConn 소멸자 오류 발생 / 커넥션이 NULL입니다.");
		jc_assert_msg(connectionPool_ != nullptr, "AutoReleaseConn 소멸자 오류 발생 / 풀이 NULL입니다.");
		connectionPool_->ReleaseConnection(connection_);
	}

private:
	IConnection* connection_;
	ConnectionPool* connectionPool_;
};

NS_END
