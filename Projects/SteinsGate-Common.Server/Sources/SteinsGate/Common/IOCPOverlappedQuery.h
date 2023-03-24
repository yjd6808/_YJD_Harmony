/*
	작성자 : 윤정도
*/

#pragma once


#include <JNetwork/IOCPOverlapped/IOCPOverlapped.h>

class MysqlDatabase;
class MysqlQueryFuture;

class IOCPOverlappedQuery : public JNetwork::IOCPOverlapped
{
public:
	IOCPOverlappedQuery(JNetwork::IOCP* iocp, MysqlDatabase* db, MysqlQueryFuture* future);
	~IOCPOverlappedQuery() override;
public:
	void Process(BOOL result, Int32UL numberOfBytesTransffered, JNetwork::IOCPPostOrder* completionKey) override;
private:
	MysqlDatabase* m_pDatabase;
	MysqlQueryFuture* m_pMysqlQueryFuture;
};
