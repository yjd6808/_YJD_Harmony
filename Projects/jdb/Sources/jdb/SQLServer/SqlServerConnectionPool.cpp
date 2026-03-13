#include "Core.h"
#include "SqlServerConnectionPool.h"

#include "SqlServerConnection.h"

USING_NS_JC;
USING_NS_STD;

NS_JDB_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
SqlServerConnectionPool::SqlServerConnectionPool(
	const String& _hostName,
	_u16 _port,
	const String& _id,
	const String& _pass,
	const String& _dbName,
	const int _maxConn)
//////////////////////////////////////////////////////////////////////////////////////////
: hostName_(_hostName)
, accountId_(_id)
, accountPass_(_pass)
, dbName_(_dbName)
, port_(_port)
, maxConnection_(_maxConn)
, currentConnectionSize_(0)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
SqlServerConnectionPool::~SqlServerConnectionPool()
{
	TerminateAllConnections();
}

//////////////////////////////////////////////////////////////////////////////////////////
bool SqlServerConnectionPool::Init(const uint32_t _initConn)
{
	NormalLockGuard guard(mutex_);
	for (int index = 0; index < static_cast<int>(_initConn); ++index)
	{
		SqlServerConnection* pConn = CreateConnection();
		if (pConn)
		{
			connectionList_.PushBack(pConn);
			++currentConnectionSize_;
		}
		else
		{
			return false;
		}
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void SqlServerConnectionPool::TerminateAllConnections()
{
	NormalLockGuard guard(mutex_);
	connectionList_.Extension().ForEach([this](SqlServerConnection* _pConnection)
	{
		TerminateConnection(_pConnection);
	});

	currentConnectionSize_ = 0;
	connectionList_.Clear();
}

//////////////////////////////////////////////////////////////////////////////////////////
void SqlServerConnectionPool::TerminateConnection(SqlServerConnection* _pConnection)
{
	if (_pConnection)
	{
		_pConnection->Disconnect();
		delete _pConnection;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
SqlServerConnection* SqlServerConnectionPool::GetConnection()
{
	SqlServerConnection* pConn = nullptr;
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

		if (pConn == nullptr)
		{
			--currentConnectionSize_;
		}

		return pConn;
	}

	pConn = CreateConnection();
	if (pConn)
	{
		++currentConnectionSize_;
		return pConn;
	}

	return nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
void SqlServerConnectionPool::ReleaseConnection(SqlServerConnection* _pConnection)
{
	if (_pConnection)
	{
		NormalLockGuard guard(mutex_);
		connectionList_.PushBack(_pConnection);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
SqlServerConnection* SqlServerConnectionPool::CreateConnection() const
{
	SqlServerConnection* pConnection = dbg_new SqlServerConnection();

	if (pConnection == nullptr)
	{
		return nullptr;
	}

	if (!pConnection->Connect(hostName_, port_, accountId_, accountPass_, dbName_))
	{
		delete pConnection;
		return nullptr;
	}

	return pConnection;
}

NS_END
