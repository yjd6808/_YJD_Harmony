/*
 * 작성자 : 윤정도
 */

#pragma once


#include <JNetwork/Host/Server.h>
#include <JNetwork/Host/SessionContainer.h>
#include <JNetwork/Host/TcpSession.h>
#include <JNetwork/Host/Listener/TcpServerEventListener.h>

#include <JNetwork/IOCP/IOCP.h>

NS_JNET_BEGIN

class IPv4EndPoint;
class TcpServer : public Server
{
public:
	TcpServer(TcpServerEventListener* eventListener, int sessionSendBufferSize, int sessionRecvBufferSize, int maxConn);
	~TcpServer() override;

	// 초기 IOCP 쓰레드 수
	// TcpServer를 상속받은 서버에서 이 함수를 오버라이딩 해서 IOCP에서 관리할 쓰레드 수를 조절 할 수 있도록 하자.
	virtual int IocpThreadCount() const;

	void SessionDisconnected(TcpSession* session);
	void SessionAccepted(TcpSession* session);
	void SessionSent(TcpSession* session, ISendPacket* sentPacket, Int32UL receivedBytes);
	void SessionReceived(TcpSession* session, ICommand* command);

	// 접속 가능한 최대 사용자 수
	virtual int MaxConnection() const { return m_pContainer->MaxConnection(); }
	bool Start(const IPv4EndPoint& localEndPoint) override;
	bool Stop() override;
	IPv4EndPoint GetBindEndPoint() const { return m_ServerSocket.GetLocalEndPoint(); }
	TcpServerEventListener* GetEventListener() { return m_pEventListener; };
protected:
	JCore::Atomic<int> m_iSessionRecvBufferSize;
	JCore::Atomic<int> m_iSessionSendBufferSize;

	TcpServerEventListener* m_pEventListener;
	SessionContainer* m_pContainer;
	IOCPPtr m_spIocp;
};

NS_JNET_END
