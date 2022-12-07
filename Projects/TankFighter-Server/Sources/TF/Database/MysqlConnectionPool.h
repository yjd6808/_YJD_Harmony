/*
	생성일 : 2019/05/17
	수정일 : 2022/04/03
	작성자 : 윤정도
 */


#pragma once


#include <JCore/Container/LinkedList.h>
#include <JCore/Sync/NormalLock.h>
#include <JCore/Primitives/String.h>

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
	static void TerminateConnection(MysqlConnection* conn);
	MysqlConnection* CreateConnection() const;
	
private:
	MysqlConnectionPool(const JCore::String& sHostname, const uint16_t& wPort, const JCore::String& sUsername, const JCore::String& sPassword, const JCore::String& sDB, uint32_t wMaxConn);
	
	JCore::String m_HostName;
	JCore::String m_UserName;
	JCore::String m_Password;
	JCore::String m_DatabaseName;
	Int16U m_DatabasePort;
	Int32U m_MaxConnection;

	int m_iCurConnSize;
	JCore::NormalLock m_Mtx;
	JCore::LinkedList<MysqlConnection*> m_ConnectionList;


	inline static MysqlConnectionPool* ms_pInstance = nullptr;
	friend class MysqlDatabase;
};

// 소멸자 호출시 컨넥션 풀로 다시 되돌림
struct AutoReleaseConnection
{
	AutoReleaseConnection(MysqlConnection* conn, MysqlConnectionPool* pool) : m_pConn(conn), m_pConnPool(pool) {}
	~AutoReleaseConnection() {
		DebugAssertMessage(m_pConn != nullptr, "AutoReleaseConn 소멸자 오류 발생 / 컨넥션이 NULL입니다.");
		DebugAssertMessage(m_pConnPool != nullptr, "AutoReleaseConn 소멸자 오류 발생 / 풀이 NULL입니다.");
		m_pConnPool->ReleaseConnection(m_pConn);
	}
private:
	MysqlConnection* m_pConn;
	MysqlConnectionPool* m_pConnPool;
};