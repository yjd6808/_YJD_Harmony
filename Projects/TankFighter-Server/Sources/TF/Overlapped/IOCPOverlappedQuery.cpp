/*
	�ۼ��� : ������
*/

#include <TF/Overlapped/IOCPOverlappedQuery.h>
#include <TF/Database/MysqlDatabase.h>

#include <JCore/AutoObject.h>

using namespace JNetwork;


IOCPOverlappedQuery::IOCPOverlappedQuery(JNetwork::IOCP* iocp, MysqlQueryFuture* future) :
	IOCPOverlapped(iocp, Type::Custom),
	m_pMysqlQueryFuture(future)
{
}

IOCPOverlappedQuery::~IOCPOverlappedQuery() = default;

void IOCPOverlappedQuery::Process(BOOL result, DWORD numberOfBytesTransffered, IOCPPostOrder* completionKey) {
	JCore::AutoPointer<MysqlQueryFuture> autoPtrRelease(m_pMysqlQueryFuture, [](MysqlQueryFuture* future) { future->Release(); });

	auto pConn = _MysqlConnPool->GetConnection();

	if (pConn == nullptr) {
		// Ǯ�� ����
		DebugAssert(false, "IOCPOverlappedQuery::Process() Failed Ŀ�ؼ� Ǯ�� ����");
		return;
	}

	m_pMysqlQueryFuture->InitializeQuery(pConn);
	m_pMysqlQueryFuture->ExecuteQuery();
	m_pMysqlQueryFuture->SetReady();
	m_pMysqlQueryFuture->CallbackExecute();
	m_pMysqlQueryFuture->Signal();

	_MysqlConnPool->ReleaseConnection(pConn);
}
