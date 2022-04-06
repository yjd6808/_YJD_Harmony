/*
	작성자 : 윤정도
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
		// 풀링 실패
		DebugAssert(false, "IOCPOverlappedQuery::Process() Failed 커넥션 풀링 실패");
		return;
	}

	m_pMysqlQueryFuture->InitializeQuery(pConn);
	m_pMysqlQueryFuture->ExecuteQuery();
	m_pMysqlQueryFuture->SetReady();
	m_pMysqlQueryFuture->CallbackExecute();
	m_pMysqlQueryFuture->Signal();

	_MysqlConnPool->ReleaseConnection(pConn);
}
