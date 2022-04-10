/*
 * 작성자 : 윤정도
 */

#pragma once

#include <JNetwork/Host/TcpSession.h>
#include <JNetwork/Host/TcpClientEventListener.h>

namespace JNetwork {

class TcpClient : public TcpSession
{
public:
	TcpClient();
	~TcpClient() override;
public:
	// 초기 IOCP 쓰레드 수
	virtual int DefaultIocpThreadCount() const;

	virtual bool ConnectAsync(const IPv4EndPoint& destination);
	void Pause();
	void Resume();
	bool Disconnect() override;
	void SetEventListener(TcpClientEventListener* listener);
protected:
	void Connected() override;
	void ConnectWait() override;
	void NotifyCommand(ICommand* cmd) override;								// 세션 입장에서는 ServerEventListener에 커맨드를 전달하고 클라이언트 입장에서는 ClientEventListener에 커맨드를 전달하도록 한다.
	void Sent(ISendPacket* sentPacket, Int32UL sentBytes) override;
protected:
	TcpClientEventListener* m_pClientEventListener;
};

} // namespace JNetwork