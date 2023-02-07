/*
 * 작성자 : 윤정도
 */

#pragma once

#include <WinSock2.h>

#include <JNetwork/IOCP/IOCP.h>
#include <JNetwork/IOCP/IOCPPostOrder.h>


NS_JNET_BEGIN

class IOCPOverlapped : public OVERLAPPED
{
public:
	enum class Type
	{
		Custom,
		Accept,
		Connect,
		Disconnect,
		Receive,
		Send
	};
public:
	IOCPOverlapped(IOCP* iocp, Type type);
	virtual ~IOCPOverlapped() = default;
public:
	virtual void Process(BOOL result, Int32UL numberOfBytesTransffered, IOCPPostOrder* completionKey) = 0;
	virtual void Release();
	Type GetType() const { return m_eType; }
	bool IsFailed(SOCKET hSocket, BOOL result, Int32UL numberOfBytesTransffered);
protected:
	Type m_eType;
	IOCP* m_pIocp;
};

NS_JNET_END