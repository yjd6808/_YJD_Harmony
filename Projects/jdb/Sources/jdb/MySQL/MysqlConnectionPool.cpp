#include "Core.h"
#include "MysqlConnectionPool.h"

#include "MysqlConnection.h"

USING_NS_JC;
USING_NS_JC;
USING_NS_STD;

NS_JDB_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
MysqlConnectionPool::MysqlConnectionPool(
	const String& _hostName,
	_u16 _port,
	const String& _id,
	const String& _pass,
	const String& _schemaName,
	const int _maxConn)
//////////////////////////////////////////////////////////////////////////////////////////
: hostName_(_hostName)
, accountId_(_id)
, accountPass_(_pass)
, schemeName_(_schemaName)
, port_(_port)
, maxConnection_(_maxConn)
, currentConnectionSize_(0)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
MysqlConnectionPool::~MysqlConnectionPool()
{
	TerminateAllConnections();
}

//////////////////////////////////////////////////////////////////////////////////////////
bool MysqlConnectionPool::Init(const uint32_t _initConn)
{
	NormalLockGuard guard(mutex_);
	for (int index = 0; index < static_cast<int>(_initConn); ++index)
	{
		MysqlConnection* pConn = CreateConnection();
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
void MysqlConnectionPool::TerminateAllConnections()
{
	NormalLockGuard guard(mutex_);
	connectionList_.Extension().ForEach([this](MysqlConnection* _pConnection)
	{
		TerminateConnection(_pConnection);
	});

	currentConnectionSize_ = 0;
	connectionList_.Clear();
}

//////////////////////////////////////////////////////////////////////////////////////////
void MysqlConnectionPool::TerminateConnection(MysqlConnection* _pConnection)
{
	if (_pConnection)
	{
		_pConnection->Disconnect();
		delete _pConnection;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
MysqlConnection* MysqlConnectionPool::GetConnection()
{
	MysqlConnection* pConn = nullptr;
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
void MysqlConnectionPool::ReleaseConnection(MysqlConnection* _pConnection)
{
	if (_pConnection)
	{
		NormalLockGuard guard(mutex_);
		connectionList_.PushBack(_pConnection);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
MysqlConnection* MysqlConnectionPool::CreateConnection() const
{
	MysqlConnection* pConnection = dbg_new MysqlConnection();

	if (pConnection == nullptr)
	{
		return nullptr;
	}

	if (!pConnection->Connect(hostName_, port_, accountId_, accountPass_, schemeName_))
	{
		delete pConnection;
		return nullptr;
	}

	return pConnection;
}

NS_END