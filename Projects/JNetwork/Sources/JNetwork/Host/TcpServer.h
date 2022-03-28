/*
	작성자 : 윤정도
*/

#pragma once


#include <JNetwork/Host/Server.h>
#include <JNetwork/Host/TcpSessionContainer.h>
#include <JNetwork/Host/TcpServerEventListener.h>

#include <JNetwork/IOCP/IOCP.h>

namespace JNetwork {

class IPv4EndPoint;
class TcpServer : public Server
{
public:
	TcpServer(int maxConn = 8);
	~TcpServer();

	// 초기 IOCP 쓰레드 수
	// TcpServer를 상속받은 서버에서 이 함수를 오버라이딩 해서 IOCP에서 관리할 쓰레드 수를 조절 할 수 있도록 하자.
	virtual int DefaultIocpThreadCount() const;		

	// 접속 가능한 최대 사용자 수
	virtual int MaxConnection() const { return m_pContainer->MaxConnection(); }
	virtual bool Start(const IPv4EndPoint& localEndPoint);
	virtual bool Stop();
	virtual void Pause();
	virtual void Resume();
	
	// 서버는 이벤트리스너 등록을 필수적으로 해주도록 함
	virtual void SetEventListener(TcpServerEventListener* listener);
	IPv4EndPoint GetBindEndPoint() { return m_ServerSocket.GetLocalEndPoint(); }
protected:
	TcpServerEventListener* m_pEventListener;
	TcpSessionContainer* m_pContainer;
	IOCP* m_pIocp;
};

} // namespace JNetwork