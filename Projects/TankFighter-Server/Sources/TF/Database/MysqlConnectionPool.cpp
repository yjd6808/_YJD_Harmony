#include <TF/Database/MysqlConnectionPool.h>
#include <TF/Database/MysqlQuery.h>
#include <TF/Util/Console.h>
#include <TF/ServerConfiguration.h>

using namespace JCore;

MysqlConnectionPool::MysqlConnectionPool(const JCore::String& sHostname, const uint16_t& wPort, const JCore::String& sUsername, const JCore::String& sPassword, const JCore::String& sDB, const uint32_t wMaxConn) :
	m_HostName(sHostname),
	m_DatabasePort(wPort),
	m_UserName(sUsername),
	m_Password(sPassword),
	m_MaxConnection(wMaxConn),
	m_DatabaseName(sDB),
	m_iCurConnSize(0)
{
	const int initConnCount = m_MaxConnection / 2;

}

MysqlConnectionPool::~MysqlConnectionPool()
{
	TerminateAllConnections();
}



bool MysqlConnectionPool::Init(const uint32_t initConn)
{
	MysqlConnection* pConn;
	CriticalSectionLockGuard guard(m_Mtx);
	for (int i = 0; i < initConn; ++i) {
		pConn = CreateConnection();
		if (pConn) {
			m_ConnectionList.PushBack(pConn);
			++m_iCurConnSize;
		} else {
			return false;
		}
	}
	return true;
}

void MysqlConnectionPool::TerminateAllConnections()
{
	CriticalSectionLockGuard guard(m_Mtx);
	m_ConnectionList.Extension().ForEach([this](MysqlConnection* conn) {
		this->TerminateConnection(conn);
	});
	
	m_iCurConnSize = 0;
	m_ConnectionList.Clear();
}

void MysqlConnectionPool::TerminateConnection(MysqlConnection* conn)
{
	if (conn)  {
		conn->Disconnect();
		delete conn;
	}
}


MysqlConnection* MysqlConnectionPool::GetConnection()
{
	MysqlConnection* pConn;
	CriticalSectionLockGuard guard(m_Mtx);

	if (m_ConnectionList.Size() > 0) 
	{
		pConn = m_ConnectionList.Front();
		m_ConnectionList.PopFront();

		if (pConn->IsConnected() == false) {
			DeleteSafe(pConn);
			pConn = this->CreateConnection();
		}

		if (pConn == nullptr) 
			--m_iCurConnSize;

		return pConn;
	}

	pConn = CreateConnection();
	if (pConn) {
		++m_iCurConnSize;
		return pConn;
	}

	return nullptr;
}

void MysqlConnectionPool::ReleaseConnection(MysqlConnection* conn)
{
	if (conn)  {
		CriticalSectionLockGuard guard(m_Mtx);
		m_ConnectionList.PushBack(conn);
	}
}

MysqlConnection* MysqlConnectionPool::CreateConnection()
{
	MysqlConnection* connection = new (std::nothrow) MysqlConnection();

	if (connection == nullptr) {
		return nullptr;
	}
	connection->Connect(m_HostName, m_DatabasePort, m_UserName, m_Password, m_DatabaseName);
	return connection;
}
