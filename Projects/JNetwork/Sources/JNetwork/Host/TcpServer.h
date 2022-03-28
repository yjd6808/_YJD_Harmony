/*
	�ۼ��� : ������
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

	// �ʱ� IOCP ������ ��
	// TcpServer�� ��ӹ��� �������� �� �Լ��� �������̵� �ؼ� IOCP���� ������ ������ ���� ���� �� �� �ֵ��� ����.
	virtual int DefaultIocpThreadCount() const;		

	// ���� ������ �ִ� ����� ��
	virtual int MaxConnection() const { return m_pContainer->MaxConnection(); }
	virtual bool Start(const IPv4EndPoint& localEndPoint);
	virtual bool Stop();
	virtual void Pause();
	virtual void Resume();
	
	// ������ �̺�Ʈ������ ����� �ʼ������� ���ֵ��� ��
	virtual void SetEventListener(TcpServerEventListener* listener);
	IPv4EndPoint GetBindEndPoint() { return m_ServerSocket.GetLocalEndPoint(); }
protected:
	TcpServerEventListener* m_pEventListener;
	TcpSessionContainer* m_pContainer;
	IOCP* m_pIocp;
};

} // namespace JNetwork