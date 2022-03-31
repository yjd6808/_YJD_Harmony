/*
	생성일 : 2019/05/17
	작성자 : 윤정도
 */


#pragma once

#include <memory>
#include <list>
#include <mutex>

class MySQLConnection;

#define DFMysqlPool MysqlConnectionPool::GetInstance()

class MysqlConnectionPool
{
public:
	MySQLConnection* GetConnection();
	void ReleaseConnection(MySQLConnection* conn);

	static MysqlConnectionPool& GetInstance();
	~MysqlConnectionPool();
private:
	bool Init(const uint32_t initConn);
	void TerminateAllConnections();
	void TerminateConnection(MySQLConnection* conn);
	MySQLConnection* CreateConnection();
	
private:
	MysqlConnectionPool(const std::string& sHostname, const uint16_t& wPort, const std::string& sUsername, const std::string& sPassword, const std::string& sDB, const uint32_t wMaxConn);
	
	std::string m_hostname;
	std::string m_username;
	std::string m_password;
	std::string m_dbname;
	uint16_t m_dbport;
	uint32_t m_maxconnection;

	int m_curConnSize;
	static std::unique_ptr<MysqlConnectionPool> s_instance;
	std::mutex m_mtx;
	std::list<MySQLConnection*> m_connList;
	friend class std::unique_ptr<MysqlConnectionPool>;
};

