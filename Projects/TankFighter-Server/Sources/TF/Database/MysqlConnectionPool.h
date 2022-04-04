/*
	생성일 : 2019/05/17
	작성자 : 윤정도
 */


#pragma once

#include <memory>
#include <list>
#include <mutex>

#include <JCore/String.h>

class MysqlConnection;

#define TFDBConnPool MysqlConnectionPool::GetInstance()

class MysqlConnectionPool
{
public:
	MysqlConnection* GetConnection();
	void ReleaseConnection(MysqlConnection* conn);

	static MysqlConnectionPool& GetInstance();
	~MysqlConnectionPool();

	bool Init(const uint32_t initConn);
private:
	void TerminateAllConnections();
	void TerminateConnection(MysqlConnection* conn);
	MysqlConnection* CreateConnection();
	
private:
	MysqlConnectionPool(const JCore::String& sHostname, const uint16_t& wPort, const JCore::String& sUsername, const JCore::String& sPassword, const JCore::String& sDB, const uint32_t wMaxConn);
	
	JCore::String m_hostname;
	JCore::String m_username;
	JCore::String m_password;
	JCore::String m_dbname;
	uint16_t m_dbport;
	uint32_t m_maxconnection;

	int m_curConnSize;
	static std::unique_ptr<MysqlConnectionPool> s_instance;
	std::mutex m_mtx;
	std::list<MysqlConnection*> m_connList;
	friend class std::unique_ptr<MysqlConnectionPool>;
};

