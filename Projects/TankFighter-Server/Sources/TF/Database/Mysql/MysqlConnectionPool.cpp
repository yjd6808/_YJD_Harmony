#include <TF/Database/Mysql/MysqlConnectionPool.h>
#include <TF/Database/DataBaseManager.h>
#include <TF/Database/Mysql/MysqlQuery.h>
#include <TF/Util/Console.h>

using namespace std;

unique_ptr<MysqlConnectionPool> MysqlConnectionPool::s_instance;

MysqlConnectionPool::MysqlConnectionPool(const std::string& sHostname, const uint16_t& wPort, const std::string& sUsername, const std::string& sPassword, const std::string& sDB, const uint32_t wMaxConn) :
	m_hostname(sHostname),
	m_dbport(wPort),
	m_username(sUsername),
	m_password(sPassword),
	m_maxconnection(wMaxConn),
	m_dbname(sDB),
	m_curConnSize(0)
{
	int initConnCount = m_maxconnection / 2;

	if (Init(initConnCount))
	{
		Console::WriteLine(ConsoleColor::CYAN, "[Info] %d Database Connections Commit Complete", initConnCount);
	}
}

MysqlConnectionPool::~MysqlConnectionPool()
{
	TerminateAllConnections();
}


MysqlConnectionPool& MysqlConnectionPool::GetInstance()
{
	if (s_instance.get() == nullptr)
		s_instance.reset(new MysqlConnectionPool(DB_HOST, DB_PORT, DB_ID, DB_PASS, DB_NAME, 50));

	return *(s_instance.get());
}

bool MysqlConnectionPool::Init(const uint32_t initConn)
{
	MySQLConnection* pConn;
	m_mtx.lock();
	for (int i = 0; i < initConn; ++i) {
		pConn = CreateConnection();
		if (pConn) {
			m_connList.push_back(pConn);
			++m_curConnSize;
		}
		else {
			return false;
		}
	}
	m_mtx.unlock();
	return true;
}

void MysqlConnectionPool::TerminateAllConnections()
{
	list<MySQLConnection*>::iterator conn;
	m_mtx.lock();
	for (conn = m_connList.begin(); conn != m_connList.end(); ++conn)
	{
		this->TerminateConnection(*conn);
	}
	m_curConnSize = 0;
	m_connList.clear();
	m_mtx.unlock();
}

void MysqlConnectionPool::TerminateConnection(MySQLConnection* conn)
{
	if (conn) 
	{
		conn->Disconnect();
		delete conn;
	}
	
}


MySQLConnection* MysqlConnectionPool::GetConnection()
{
	MySQLConnection* pConn;
	std::lock_guard<std::mutex> guard(m_mtx);

	if (m_connList.size() > 0) 
	{
		pConn = m_connList.front();
		m_connList.pop_front();

		if (pConn->IsConnected() == false)
		{
			delete pConn;
			pConn = this->CreateConnection();
		}

		if (pConn == nullptr) 
			--m_curConnSize;
		return pConn;
	}
	else 
	{
		pConn = CreateConnection();
		if (pConn) 
		{
			m_connList.push_back(pConn);
			++m_curConnSize;
		}
		else 
		{
			return nullptr;
		}
	}
	return nullptr;
}

void MysqlConnectionPool::ReleaseConnection(MySQLConnection* conn)
{
	if (conn) 
	{
		m_mtx.lock();
		m_connList.push_back(conn);
		m_mtx.unlock();
	}
}

MySQLConnection* MysqlConnectionPool::CreateConnection()
{
	MySQLConnection* connection = new MySQLConnection();
	connection->Connect(m_hostname, m_dbport, m_username, m_password, m_dbname);
	return connection;
}
