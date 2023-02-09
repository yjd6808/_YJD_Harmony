/*
	작성자 : 윤정도
*/

#include <TF/PrecompiledHeader.h>
#include <TF/Overlapped/IOCPOverlappedQuery.h>
#include <TF/Database/MysqlDatabase.h>

#include <JCore/RAII/AutoPtr.h>

using namespace JNetwork;


IOCPOverlappedQuery::IOCPOverlappedQuery(IOCP* iocp, MysqlQueryFuture* future) :
	IOCPOverlapped(iocp, Type::Custom),
	m_pMysqlQueryFuture(future)
{
}

IOCPOverlappedQuery::~IOCPOverlappedQuery() = default;

void IOCPOverlappedQuery::Process(BOOL result, Int32UL numberOfBytesTransffered, IOCPPostOrder* completionKey) {
	JCore::AutoPtr<MysqlQueryFuture> autoReleaseFuture(m_pMysqlQueryFuture, [](MysqlQueryFuture* future) { future->Release(); });

	auto pConn = _MysqlConnPool->GetConnection();

	if (pConn == nullptr) {
		// 풀링 실패
		DebugAssertMsg(false, "IOCPOverlappedQuery::Process() Failed 커넥션 풀링 실패");
		return;
	}

	AutoReleaseConnection autoReleaseConnection(pConn, _MysqlConnPool);

	m_pMysqlQueryFuture->InitializeQuery(pConn);
	m_pMysqlQueryFuture->ExecuteQuery();
	m_pMysqlQueryFuture->SetReady();
	m_pMysqlQueryFuture->CallbackExecute();
	m_pMysqlQueryFuture->Signal();
}
