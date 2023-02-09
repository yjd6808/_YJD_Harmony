#include <TF/PrecompiledHeader.h>
#include <TF/Database/MysqlConnectionPool.h>
#include <TF/Database/MysqlQuery.h>
#include <JCore/Utils/Console.h>

using namespace JCore;

MysqlConnectionPool::MysqlConnectionPool(const String& sHostname, const uint16_t& wPort, const String& sUsername, const
                                         String& sPassword, const String& sDB, const uint32_t wMaxConn) :
	m_HostName(sHostname),
	m_UserName(sUsername),
	m_Password(sPassword),
	m_DatabaseName(sDB),
	m_DatabasePort(wPort),
	m_MaxConnection(wMaxConn),
	m_iCurConnSize(0)
{
}

MysqlConnectionPool::~MysqlConnectionPool() {
	TerminateAllConnections();
}



bool MysqlConnectionPool::Init(const uint32_t initConn) {
	NormalLockGuard guard(m_Mtx);
	for (int i = 0; i < initConn; ++i) {
		MysqlConnection* pConn = CreateConnection();
		if (pConn) {
			m_ConnectionList.PushBack(pConn);
			++m_iCurConnSize;
		} else {
			return false;
		}
	}
	return true;
}

void MysqlConnectionPool::TerminateAllConnections() {
	NormalLockGuard guard(m_Mtx);
	m_ConnectionList.Extension().ForEach([this](MysqlConnection* conn) {
		this->TerminateConnection(conn);
	});
	
	m_iCurConnSize = 0;
	m_ConnectionList.Clear();
}

void MysqlConnectionPool::TerminateConnection(MysqlConnection* conn) {
	if (conn)  {
		conn->Disconnect();
		delete conn;
	}
}


MysqlConnection* MysqlConnectionPool::GetConnection() {
	MysqlConnection* pConn;
	NormalLockGuard guard(m_Mtx);

	if (m_ConnectionList.Size() > 0) {
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

void MysqlConnectionPool::ReleaseConnection(MysqlConnection* conn) {
	if (conn)  {
		NormalLockGuard guard(m_Mtx);
		m_ConnectionList.PushBack(conn);
	}
}

MysqlConnection* MysqlConnectionPool::CreateConnection() const {
	MysqlConnection* connection = new (std::nothrow) MysqlConnection();

	if (connection == nullptr) {
		return nullptr;
	}
	connection->Connect(m_HostName, m_DatabasePort, m_UserName, m_Password, m_DatabaseName);
	return connection;
}
