/*
	작성자 : 윤정도
*/

#include "Server.h"
#include "IOCPOverlappedQuery.h"

#include <SteinsGate/Common/MysqlDatabase.h>
#include <JCore/RAII/AutoPtr.h>

USING_NS_JNET;

IOCPOverlappedQuery::IOCPOverlappedQuery(IOCP* iocp, MysqlDatabase* db, MysqlQueryFuture* future)
	: IOCPOverlapped(iocp, Type::Custom)
	, m_pMysqlQueryFuture(future)
	, m_pDatabase(db)
{}

IOCPOverlappedQuery::~IOCPOverlappedQuery() = default;

void IOCPOverlappedQuery::Process(BOOL result, Int32UL numberOfBytesTransffered, IOCPPostOrder* completionKey) {
	JCore::AutoPtr<MysqlQueryFuture> autoReleaseFuture(m_pMysqlQueryFuture, [](MysqlQueryFuture* future) { future->Release(); });

	MysqlConnectionPool* pPool = m_pDatabase->GetConnectionPool();
	MysqlConnection* pConn = pPool->GetConnection();

	if (pConn == nullptr) {
		// 풀링 실패
		_LogError_("IOCPOverlappedQuery::Process() Failed 커넥션 풀링 실패");
		return;
	}

	AutoReleaseConnection autoReleaseConnection(pConn, pPool);

	m_pMysqlQueryFuture->InitializeQuery(pConn);
	m_pMysqlQueryFuture->ExecuteQuery();
	m_pMysqlQueryFuture->SetReady();
	m_pMysqlQueryFuture->CallbackExecute();
	m_pMysqlQueryFuture->Signal();
}
