/*
	�ۼ��� : ������
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
	// �ʱ� IOCP ������ ��
	virtual int DefaultIocpThreadCount() const;

	bool ConnectAsync(const IPv4EndPoint& localEndPoint);
	void Pause();
	void Resume();
	virtual bool Disconnect();
	void SetEventListener(TcpClientEventListener* listener);
protected:
	virtual void Connected();
	virtual void ConnectWait();
	virtual void NotifyCommand(ICommand* cmd);								// ���� ���忡���� ServerEventListener�� Ŀ�ǵ带 �����ϰ� Ŭ���̾�Ʈ ���忡���� ClientEventListener�� Ŀ�ǵ带 �����ϵ��� �Ѵ�.
	virtual void Sent(ISendPacket* sentPacket, Int32UL sentBytes);
protected:
	TcpClientEventListener* m_pClientEventListener;
};

} // namespace JNetwork