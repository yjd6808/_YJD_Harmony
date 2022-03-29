/*
	작성자 : 윤정도
*/

#pragma once

#include <JNetwork/Host/TcpSession.h>
#include <JNetwork/Host/TcpClientEventListener.h>

namespace JNetwork {

class TcpClient : public TcpSession
{
public:
	TcpClient();
	virtual ~TcpClient();
public:
	// 초기 IOCP 쓰레드 수
	virtual int DefaultIocpThreadCount() const;

	bool ConnectAsync(const IPv4EndPoint& localEndPoint);
	void Pause();
	void Resume();
	virtual bool Disconnect();
	void SetEventListener(TcpClientEventListener* listener);
protected:
	virtual void Connected();
	virtual void ConnectWait();
	virtual void NotifyCommand(ICommand* cmd);								// 세션 입장에서는 ServerEventListener에 커맨드를 전달하고 클라이언트 입장에서는 ClientEventListener에 커맨드를 전달하도록 한다.
	virtual void Sent(ISendPacket* sentPacket, Int32UL sentBytes);
protected:
	TcpClientEventListener* m_pClientEventListener;
};

} // namespace JNetwork