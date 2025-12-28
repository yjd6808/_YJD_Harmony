/*
 * 작성자 : 윤정도
 */

#pragma once


#include <JNetwork/Host/Server.h>
#include <JNetwork/Host/ISessionContainer.h>
#include <JNetwork/Host/TcpSession.h>
#include <JNetwork/EventListener/ServerEventListener.h>

#include <JNetwork/IOCP/IOCP.h>
#include <JCore/Sync/NormalLock.h>

NS_JNET_BEGIN

class IPv4EndPoint;
class TcpServer : public Server
{
public:
	TcpServer(const IOCPPtr& _pIocp, const JCore::MemoryPoolAbstractPtr& _pBufferAllocator);
	~TcpServer() override;

	// 커스텀 생성을 위한 버철
	virtual TcpSession* CreateSession();
	virtual ISessionContainer* CreateSessionContainer();

	virtual void OnStarted() {}
	virtual void OnStartFailed(Int32U _errorCode) {}
	virtual void OnStopped() {}

	ISessionContainer* GetSessionContainer();
	ServerEventListener* GetEventListener();

	void SetSesssionContainer(ISessionContainer* _pContainer);
	void SetEventListener(ServerEventListener* _pListener);

	bool Start(const IPv4EndPoint& _localEndPoint) override;
	bool Stop() override;

	void Initialize() override;
	void SessionDisconnected(TcpSession* _pSession, Int32U _errorCode);
	void SessionConnected(TcpSession* _pSession);
	void SessionConnectFailed(TcpSession* _pSession, Int32U _errorCode);
	void SessionSent(TcpSession* _pSession, IPacket* _pSentPacket, Int32UL _receivedBytes);
	void SessionReceived(TcpSession* _pSession, ICommand* _pCommand);
	void SessionReceived(TcpSession* _pSession, RecvedCommandPacket* _pRecvPacket);
	void SessionReceivedRaw(TcpSession* _pSession, char* _pData, int _len);

	IPv4EndPoint GetBindEndPoint() const { return socket_.GetLocalEndPoint(); }
	const char* TypeName() override { return "TCP 서버"; }
	DetailType GetDetailType() const override { return eTcpListener; }

protected:
	JCore::MemoryPoolAbstractPtr pBufferAllocator_;
	JCore::NormalLock lock_;

	ServerEventListener* pServerEventListener_;
	ISessionContainer* pSessionContainer_;
};

using TcpServerPtr = JCore::SharedPtr<TcpServer>;
using TcpServerWPtr = JCore::WeakPtr<TcpServer>;

NS_JNET_END
