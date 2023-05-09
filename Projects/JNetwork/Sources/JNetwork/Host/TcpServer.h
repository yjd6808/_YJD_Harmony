/*
 * 작성자 : 윤정도
 */

#pragma once


#include <JNetwork/Host/Server.h>
#include <JNetwork/Host/SessionContainer.h>
#include <JNetwork/Host/TcpSession.h>
#include <JNetwork/EventListener/ServerEventListener.h>

#include <JNetwork/IOCP/IOCP.h>

NS_JNET_BEGIN

class IPv4EndPoint;
class TcpServer : public Server
{
public:
	TcpServer(
		const IOCPPtr& iocp,
		const JCore::MemoryPoolAbstractPtr& bufferAllocator,
		ServerEventListener* eventListener,
		int maxConn = 10,
		int sessionRecvBufferSize = 6000, 
		int sessionSendBufferSize = 6000
	);
	~TcpServer() override;


	// 커스텀 세션 생성을 위한 버철
	virtual TcpSession* CreateSession();

	bool Start(const IPv4EndPoint& localEndPoint) override;
	bool Stop() override;

	void Initialize() override;
	void SessionDisconnected(TcpSession* session);
	void SessionConnected(TcpSession* session);
	void SessionSent(TcpSession* session, ISendPacket* sentPacket, Int32UL receivedBytes);
	void SessionReceived(TcpSession* session, ICommand* command);
	void BroadcastAsync(ISendPacket* packet);

	int GetMaxConnection() const { return m_pContainer->MaxConnection(); }
	IPv4EndPoint GetBindEndPoint() const { return m_Socket.GetLocalEndPoint(); }
	SessionContainer* GetSessionContainer() const { return m_pContainer; }
protected:
	JCore::Atomic<int> m_iSessionRecvBufferSize;
	JCore::Atomic<int> m_iSessionSendBufferSize;
	JCore::MemoryPoolAbstractPtr m_spBufferAllocator;

	ServerEventListener* m_pEventListener;
	SessionContainer* m_pContainer;
};

using TcpServerPtr = JCore::SharedPtr<TcpServer>;
using TcpServerWPtr = JCore::WeakPtr<TcpServer>;

NS_JNET_END
