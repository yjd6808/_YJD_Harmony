/*
 * �ۼ��� : ������
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
	// �ʱ� IOCP ������ ��
	virtual int DefaultIocpThreadCount() const;

	virtual bool ConnectAsync(const IPv4EndPoint& destination);
	void Pause();
	void Resume();
	bool Disconnect() override;
	void SetEventListener(TcpClientEventListener* listener);
protected:
	void Connected() override;
	void ConnectWait() override;
	void NotifyCommand(ICommand* cmd) override;								// ���� ���忡���� ServerEventListener�� Ŀ�ǵ带 �����ϰ� Ŭ���̾�Ʈ ���忡���� ClientEventListener�� Ŀ�ǵ带 �����ϵ��� �Ѵ�.
	void Sent(ISendPacket* sentPacket, Int32UL sentBytes) override;
protected:
	TcpClientEventListener* m_pClientEventListener;
};

} // namespace JNetwork