/*
	작성자 : 윤정도
*/

#pragma once

#include <WinSock2.h>

#include <JNetwork/IOCP/IOCP.h>
#include <JNetwork/IOCP/IOCPPostOrder.h>

namespace JNetwork {

class IOCPOverlapped : public OVERLAPPED
{
public:
	enum class Type
	{
		Custom,
		Accept,
		Connect,
		Receive,
		Send
	};
public:
	IOCPOverlapped(IOCP* iocp, Type type) : m_eType(type), m_pIocp(iocp) {}
	virtual ~IOCPOverlapped() = default;
public:
	virtual void Process(BOOL result, DWORD numberOfBytesTransffered, IOCPPostOrder* completionKey) = 0;
	virtual void Release() { delete this; }

	Type GetType() const { return m_eType; }
	bool IsFailed(SOCKET hSocket, BOOL result, DWORD numberOfBytesTransffered);
protected:
	Type m_eType;
	IOCP* m_pIocp;
};

} // namespace JNetwork