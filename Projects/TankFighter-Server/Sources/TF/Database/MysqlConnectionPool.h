/*
	������ : 2019/05/17
	������ : 2022/04/03
	�ۼ��� : ������
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
	JCore::CriticalSectionMutex m_Mtx;
	JCore::LinkedList<MysqlConnection*> m_ConnectionList;


	inline static MysqlConnectionPool* ms_pInstance = nullptr;
	friend class MysqlDatabase;
};

// �Ҹ��� ȣ��� ���ؼ� Ǯ�� �ٽ� �ǵ���
struct AutoReleaseConnection
{
	AutoReleaseConnection(MysqlConnection* conn, MysqlConnectionPool* pool) : m_pConn(conn), m_pConnPool(pool) {}
	~AutoReleaseConnection() {
		DebugAssert(m_pConn != nullptr, "AutoReleaseConn �Ҹ��� ���� �߻� / ���ؼ��� NULL�Դϴ�.");
		DebugAssert(m_pConnPool != nullptr, "AutoReleaseConn �Ҹ��� ���� �߻� / Ǯ�� NULL�Դϴ�.");
		m_pConnPool->ReleaseConnection(m_pConn);
	}
private:
	MysqlConnection* m_pConn;
	MysqlConnectionPool* m_pConnPool;
};