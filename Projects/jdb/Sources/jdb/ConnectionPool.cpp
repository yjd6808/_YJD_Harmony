/*
 * 작성자: 윤정도
 * 생성일: 3/13/2026
 * =====================
 *
 * 통합 커넥션 풀 구현
 */

#include "ConnectionPool.h"

#include "IConnection.h"

#include "SQLServer/SqlServerConnection.h"

USING_NS_JC;
USING_NS_STD;

NS_JDB_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
ConnectionPool::ConnectionPool(
	DatabaseType _dbType,
	const String& _hostName,
	_u16 _port,
	const String& _id,
	const String& _pass,
	const String& _dbName,
	const int _maxConn)
//////////////////////////////////////////////////////////////////////////////////////////
: dbType_(_dbType)
, hostName_(_hostName)
, accountId_(_id)
, accountPass_(_pass)
, dbName_(_dbName)
, port_(_port)
, maxConnection_(_maxConn)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
ConnectionPool::~ConnectionPool()
{
	TerminateAllConnections();
}

//////////////////////////////////////////////////////////////////////////////////////////
bool ConnectionPool::Init(const uint32_t _initConn)
{
	NormalLockGuard guard(mutex_);
	for (int index = 0; index < static_cast<int>(_initConn); ++index)
	{
		IConnection* pConn = CreateConnection();
		if (pConn)
		{
			connectionList_.PushBack(pConn);
		}
		else
		{
			return false;
		}
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void ConnectionPool::TerminateAllConnections()
{
	NormalLockGuard guard(mutex_);
	connectionList_.Extension().ForEach([this](IConnection* _pConnection)
	{
		TerminateConnection(_pConnection);
	});

	connectionList_.Clear();
}

//////////////////////////////////////////////////////////////////////////////////////////
void ConnectionPool::TerminateConnection(IConnection* _pConnection)
{
	if (_pConnection)
	{
		_pConnection->Disconnect();
		delete _pConnection;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
IConnection* ConnectionPool::GetConnection()
{
	IConnection* pConn = nullptr;
	NormalLockGuard guard(mutex_);

	if (connectionList_.Size() > 0)
	{
		pConn = connectionList_.Front();
		connectionList_.PopFront();

		if (!pConn->IsConnected())
		{
			JC_DELETE_SAFE(pConn);
			pConn = CreateConnection();
		}

		if (pConn != nullptr)
		{
			++usingConnCount_;
		}

		return pConn;
	}

	pConn = CreateConnection();
	if (pConn)
	{
		++usingConnCount_;
		return pConn;
	}

	return nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
void ConnectionPool::ReleaseConnection(IConnection* _pConnection)
{
	if (_pConnection)
	{
		NormalLockGuard guard(mutex_);
		connectionList_.PushBack(_pConnection);
		--usingConnCount_;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ConnectionPool::GetConnCount(OUT int& _usingConnCount, OUT int& _availableConnCount) const
{
	NormalLockGuard guard(mutex_);
	_usingConnCount = usingConnCount_;
	_availableConnCount = connectionList_.Size();
}

//////////////////////////////////////////////////////////////////////////////////////////
IConnection* ConnectionPool::CreateConnection() const
{
	IConnection* pConnection = nullptr;

	if (dbType_ == DatabaseType::dbtMySQL)
	{
		pConnection = dbg_new MysqlConnection();
	}
	else
	{
		pConnection = dbg_new SqlServerConnection();
	}

	if (pConnection == nullptr)
		return nullptr;

	if (!pConnection->Connect(hostName_, port_, accountId_, accountPass_, dbName_))
	{
		delete pConnection;
		return nullptr;
	}

	return pConnection;
}

NS_END
