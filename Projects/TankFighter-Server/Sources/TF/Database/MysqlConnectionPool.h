/*
	생성일 : 2019/05/17
	작성자 : 윤정도
 */


#pragma once


#include <JCore/Container/LinkedList.h>
#include <JCore/LockGuard.h>
#include <JCore/String.h>

class MysqlConnection;


class MysqlConnectionPool
{
public:
	MysqlConnection* GetConnection();
	void ReleaseConnection(MysqlConnection* conn);
	~MysqlConnectionPool();

	bool Init(const uint32_t initConn);
private:
	void TerminateAllConnections();
	void TerminateConnection(MysqlConnection* conn);
	MysqlConnection* CreateConnection();
	
private:
	MysqlConnectionPool(const JCore::String& sHostname, const uint16_t& wPort, const JCore::String& sUsername, const JCore::String& sPassword, const JCore::String& sDB, const uint32_t wMaxConn);
	
	JCore::String m_HostName;
	JCore::String m_UserName;
	JCore::String m_Password;
	JCore::String m_DatabaseName;
	uint16_t m_DatabasePort;
	uint32_t m_MaxConnection;

	int m_iCurConnSize;
	inline static MysqlConnectionPool* ms_pInstance = nullptr;
	JCore::CriticalSectionMutex m_Mtx;
	JCore::LinkedList<MysqlConnection*> m_ConnectionList;

	friend class MysqlDatabase;
};

