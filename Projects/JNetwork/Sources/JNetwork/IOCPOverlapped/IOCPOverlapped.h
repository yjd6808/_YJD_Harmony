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
	IOCPOverlapped(IOCP* _pIocp, Type _type);
	~IOCPOverlapped() override;

public:
	virtual void Process(BOOL _result, Int32UL _bytesTransferred, IOCPPostOrder* _pCompletionKey) = 0;
	Type GetType() const { return type_; }

	void ReleaseAction() override { delete this; } // TODO: 풀링
	bool IsFailed(BOOL _result, OUT Int32U& _errorCode);
	bool IsFailed(SOCKET _socket, BOOL _result, Int32UL _bytesTransferred, OUT Int32U& _errorCode);

protected:
	Type type_;
	IOCP* pIocp_;
};

using IOCPOverlappedPtr = JCore::SharedPtr<IOCPOverlapped>;

NS_JNET_END
