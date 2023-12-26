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
	TcpServer(const IOCPPtr& iocp, const JCore::MemoryPoolAbstractPtr& bufferAllocator);
	~TcpServer() override;


	// 커스텀 생성을 위한 버철
	virtual TcpSession* CreateSession();
	virtual ISessionContainer* CreateSessionContainer();

	virtual void OnStarted() {}
	virtual void OnStartFailed(Int32U errorCode) {}
	virtual void OnStopped() {}

	ISessionContainer* GetSessionContainer();
	ServerEventListener* GetEventListener();

	void SetSesssionContainer(ISessionContainer* container);
	void SetEventListener(ServerEventListener* listener);

	bool Start(const IPv4EndPoint& localEndPoint) override;
	bool Stop() override;

	void Initialize() override;
	void SessionDisconnected(TcpSession* session);
	void SessionConnected(TcpSession* session);
	void SessionConnectFailed(TcpSession* session, Int32U errorCode);
	void SessionSent(TcpSession* session, IPacket* sentPacket, Int32UL receivedBytes);
	void SessionReceived(TcpSession* session, ICommand* command);
	void SessionReceived(TcpSession* session, RecvedCommandPacket* recvPacket);
	void SessionReceivedRaw(TcpSession* session, char* data, int len);

	IPv4EndPoint GetBindEndPoint() const { return m_Socket.GetLocalEndPoint(); }
	const char* TypeName() override { return "TCP 서버"; }
	DetailType GetDetailType() const override { return eTcpListener; }
protected:
	JCore::MemoryPoolAbstractPtr m_spBufferAllocator;
	JCore::NormalLock m_Sync;

	ServerEventListener* m_pEventListener;
	ISessionContainer* m_pContainer;
};

using TcpServerPtr = JCore::SharedPtr<TcpServer>;
using TcpServerWPtr = JCore::WeakPtr<TcpServer>;

NS_JNET_END
