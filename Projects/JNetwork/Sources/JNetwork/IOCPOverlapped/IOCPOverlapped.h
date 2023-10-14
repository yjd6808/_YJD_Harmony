/*
 * 작성자 : 윤정도
 */

#pragma once

#include <WinSock2.h>

#include <JCore/Primitives/RefCountObject.h>

#include <JNetwork/IOCP/IOCP.h>
#include <JNetwork/IOCP/IOCPPostOrder.h>


NS_JNET_BEGIN

class IOCPOverlapped
	: public OVERLAPPED
	, public JCore::RefCountObject
{
public:
	enum class Type
	{
		Custom,
		Accept,
		Connect,
		Disconnect,
		Receive,
		Send,
		ReceiveFrom,
		SendTo
	};
public:
	IOCPOverlapped(IOCP* iocp, Type type);
	~IOCPOverlapped() override;
public:
	virtual void Process(BOOL result, Int32UL bytesTransffered, IOCPPostOrder* completionKey) = 0;
	Type GetType() const { return m_eType; }

	void ReleaseAction() override { delete this; }	// TODO: 풀링
	bool IsFailed(BOOL result, JCORE_OUT Int32U& errorCode);
	bool IsFailed(SOCKET hSocket, BOOL result, Int32UL bytesTransffered, JCORE_OUT Int32U& errorCode);
protected:
	Type m_eType;
	IOCP* m_pIocp;
};

using IOCPOverlappedPtr = JCore::SharedPtr<IOCPOverlapped>;

NS_JNET_END