/*
	?ۼ??? : ??????
*/

#pragma once


#include <JNetwork/IOCPOverlapped/IOCPOverlapped.h>

#include <TF/Database/MysqlQueryFuture.h>


class IOCPOverlappedQuery : public JNetwork::IOCPOverlapped
{
public:
	IOCPOverlappedQuery(JNetwork::IOCP* iocp, MysqlQueryFuture* future);
	~IOCPOverlappedQuery() override;
public:
	void Process(BOOL result, DWORD numberOfBytesTransffered, JNetwork::IOCPPostOrder* completionKey) override;
private:
	MysqlQueryFuture* m_pMysqlQueryFuture;
};
