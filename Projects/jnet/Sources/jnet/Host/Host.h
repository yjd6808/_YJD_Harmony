/*
 * 작성자: 윤정도
 * 생성일: 2/9/2023 1:12:18 PM
 * =====================
 * IOCP 이용자의 공통 기능 정의
 */


#pragma once

#include <jc/Container/Properties.h>
#include <jc/Sync/SpinLock.h>

#include <jnet/Socket.h>
#include <jnet/IOCP/IOCP.h>

NS_JNET_BEGIN
class JC_NOVTABLE Host
{
public:
	using PropertyKeyType = int;

	Host(const IOCPPtr& _pIocp)
	: handle_(-1)
	, pIocp_(_pIocp)
	, socket_(TransportProtocol::None, INVALID_SOCKET)
	, state_(eNone)
	, iocpConnected_(false)
	{
	}

	virtual ~Host() = default;

	enum Type
	{
		eListener,
		eClient,
		eSession
	};

	enum DetailType
	{
		eTcpListener,
		eTcpSession,
		eTcpClient,
		eUdpClient
	};

	enum State
	{
		eNone = 0,
		eInitailized = 1,
		eAcceptWait = 2,
		eConnectWait = 2,
		eListening = 3,
		eAccepted = 3,
		eConnected = 3,
		eDisconnected = 4,
		eStopped = 4
	};

	virtual void Initialize() = 0;
	virtual Type GetType() const = 0;
	virtual const char* GetName() const { return TypeName(); } // 호스트 이름을 디폴트로 함.
	virtual DetailType GetDetailType() const = 0;
	virtual const char* TypeName() const = 0;

	State GetState()
	{
		return static_cast<State>(state_.Load());
	}

	bool IsConnected()
	{
		return state_.Load() == eConnected;
	}

	bool CreateSocket(TransportProtocol _protocol, bool _nonBlocking = true);
	bool ConnectIocp();

	const Socketv4& Socket() const
	{
		return socket_;
	}

	SOCKET SocketHandle() const
	{
		return socket_.Handle;
	}

	bool IsTCP() const
	{
		return socket_.Protocol == TransportProtocol::TCP;
	}

	bool IsUDP() const
	{
		return socket_.Protocol == TransportProtocol::UDP;
	}

	TransportProtocol Protocol() const
	{
		return socket_.Protocol;
	}

	void SetHandle(object_id _handle) { handle_ = _handle; }
	object_id GetHandle() const { return handle_; }
protected:
	object_id handle_;

	IOCPPtr pIocp_;
	Socketv4 socket_;
	jc::AtomicInt state_;
	bool iocpConnected_;

	jc::SpinLock propLock_;
	jc::Properties<PropertyKeyType> props_;
};

using HostPtr = jc::SharedPtr<Host>;

NS_END
