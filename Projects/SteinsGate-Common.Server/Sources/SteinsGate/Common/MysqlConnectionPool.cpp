#include "Server.h"
#include "MysqlConnectionPool.h"

#include <SteinsGate/Common/MysqlConnection.h>

USING_NS_JC;
USING_NS_JC;
USING_NS_STD;

MysqlConnectionPool::MysqlConnectionPool(
	const String& hostname, 
	Int16U port,
	const String& id, 
	const String& pass, 
	const String& schemaName, 
	const int maxConn
) :
	m_HostName(hostname),
	m_AccountId(id),
	m_AccountPass(pass),
	m_SchemeName(schemaName),
	m_iPort(port),
	m_MaxConnection(maxConn),
	m_iCurConnSize(0)
{}

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
	MysqlConnection* connection = dbg_new MysqlConnection();

	if (connection == nullptr) {
		return nullptr;
	}

	if (!connection->Connect(m_HostName, m_iPort, m_AccountId, m_AccountPass, m_SchemeName)) {
		delete connection;
		return nullptr;
	}

	return connection;
}
