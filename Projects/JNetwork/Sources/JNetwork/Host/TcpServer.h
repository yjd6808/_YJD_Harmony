/*
 * �ۼ��� : ������
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
	~TcpServer() override;

	// �ʱ� IOCP ������ ��
	// TcpServer�� ��ӹ��� �������� �� �Լ��� �������̵� �ؼ� IOCP���� ������ ������ ���� ���� �� �� �ֵ��� ����.
	virtual int DefaultIocpThreadCount() const;		

	// ���� ������ �ִ� ����� ��
	virtual int MaxConnection() const { return m_pContainer->MaxConnection(); }
	bool Start(const IPv4EndPoint& localEndPoint) override;
	bool Stop() override;
	virtual void Pause();
	virtual void Resume();
	
	// ������ �̺�Ʈ������ ����� �ʼ������� ���ֵ��� ��
	virtual void SetEventListener(TcpServerEventListener* listener);
	IPv4EndPoint GetBindEndPoint() const { return m_ServerSocket.GetLocalEndPoint(); }
protected:
	TcpServerEventListener* m_pEventListener;
	TcpSessionContainer* m_pContainer;
	IOCP* m_pIocp;
};

} // namespace JNetwork