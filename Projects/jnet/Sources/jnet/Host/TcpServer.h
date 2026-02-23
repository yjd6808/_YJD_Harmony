/*
 * 작성자 : 윤정도
 */

#pragma once


#include <jnet/Host/Server.h>
#include <jnet/Host/SessionContainer.h>
#include <jnet/Host/TcpSession.h>
#include <jnet/EventListener/ServerEventListener.h>

#include <jnet/IOCP/IOCP.h>
#include <jc/Sync/NormalLock.h>

NS_JNET_BEGIN

class IPv4EndPoint;
class TcpServer : public Server
{
public:
	TcpServer(const IOCPPtr& _pIocp, const jc::MemoryPoolAbstractPtr& _pBufferAllocator);
	~TcpServer() override;

	// 커스텀 생성을 위한 버철 (안씀 말고)
	virtual TcpSession* CreateSession();
	virtual SessionContainer* CreateSessionContainer();

	virtual void OnStarted() {}
	virtual void OnStartFailed(_u32 _errorCode) {}
	virtual void OnStopped() {}

	SessionContainer* GetSessionContainer();
	ServerEventListener* GetEventListener();

	void SetSesssionContainer(SessionContainer* _pContainer);
	void SetEventListener(ServerEventListener* _pListener);

	bool Start(const IPv4EndPoint& _localEndPoint) override;
	bool Stop() override;

	void Initialize() override;
	void SessionDisconnected(TcpSession* _pSession, _u32 _errorCode);
	void SessionConnected(TcpSession* _pSession);
	void SessionConnectFailed(TcpSession* _pSession, _u32 _errorCode);
	void SessionSent(TcpSession* _pSession, IPacket* _pSentPacket, _u32l _receivedBytes);
	void SessionReceived(TcpSession* _pSession, ICommand* _pCommand);
	void SessionReceived(TcpSession* _pSession, RecvedCommandPacket* _pRecvPacket);
	void SessionReceivedRaw(TcpSession* _pSession, char* _pData, int _len);

	IPv4EndPoint GetBindEndPoint() const { return socket_.GetLocalEndPoint(); }
	const char* TypeName() const override { return "TCP 서버"; }
	DetailType GetDetailType() const override { return eTcpListener; }

protected:
	jc::MemoryPoolAbstractPtr pBufferAllocator_;
	jc::NormalLock lock_;

	ServerEventListener* pServerEventListener_;
	SessionContainer* pSessionContainer_;
};

using TcpServerPtr = jc::SharedPtr<TcpServer>;
using TcpServerWPtr = jc::WeakPtr<TcpServer>;

NS_END
