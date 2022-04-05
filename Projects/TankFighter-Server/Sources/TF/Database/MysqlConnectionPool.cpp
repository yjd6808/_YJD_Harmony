#include <TF/Database/MysqlConnectionPool.h>
#include <TF/Database/MysqlQuery.h>
#include <TF/Util/Console.h>
#include <TF/ServerConfiguration.h>

using namespace std;

unique_ptr<MysqlConnectionPool> MysqlConnectionPool::ms_spInstance;

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


MysqlConnectionPool& MysqlConnectionPool::GetInstance()
{
	if (ms_spInstance.get() == nullptr)
		ms_spInstance.reset(new MysqlConnectionPool(DB_HOST, DB_PORT, DB_ID, DB_PASS, DB_NAME, 50));

	return *(ms_spInstance.get());
}

bool MysqlConnectionPool::Init(const uint32_t initConn)
{
	MysqlConnection* pConn;
	m_Mtx.lock();
	for (int i = 0; i < initConn; ++i) {
		pConn = CreateConnection();
		if (pConn) {
			m_ConnectionList.push_back(pConn);
			++m_iCurConnSize;
		}
		else {
			return false;
		}
	}
	m_Mtx.unlock();
	return true;
}

void MysqlConnectionPool::TerminateAllConnections()
{
	list<MysqlConnection*>::iterator conn;
	m_Mtx.lock();
	for (conn = m_ConnectionList.begin(); conn != m_ConnectionList.end(); ++conn)
	{
		this->TerminateConnection(*conn);
	}
	m_iCurConnSize = 0;
	m_ConnectionList.clear();
	m_Mtx.unlock();
}

void MysqlConnectionPool::TerminateConnection(MysqlConnection* conn)
{
	if (conn) 
	{
		conn->Disconnect();
		delete conn;
	}
	
}


MysqlConnection* MysqlConnectionPool::GetConnection()
{
	MysqlConnection* pConn;
	std::lock_guard<std::mutex> guard(m_Mtx);

	if (m_ConnectionList.size() > 0) 
	{
		pConn = m_ConnectionList.front();
		m_ConnectionList.pop_front();

		if (pConn->IsConnected() == false)
		{
			delete pConn;
			pConn = this->CreateConnection();
		}

		if (pConn == nullptr) 
			--m_iCurConnSize;
		return pConn;
	}
	else 
	{
		pConn = CreateConnection();
		if (pConn) 
		{
			m_ConnectionList.push_back(pConn);
			++m_iCurConnSize;
		}
		else 
		{
			return nullptr;
		}
	}
	return nullptr;
}

void MysqlConnectionPool::ReleaseConnection(MysqlConnection* conn)
{
	if (conn) 
	{
		m_Mtx.lock();
		m_ConnectionList.push_back(conn);
		m_Mtx.unlock();
	}
}

MysqlConnection* MysqlConnectionPool::CreateConnection()
{
	MysqlConnection* connection = new MysqlConnection();
	connection->Connect(m_HostName, m_DatabasePort, m_UserName, m_Password, m_DatabaseName);
	return connection;
}
