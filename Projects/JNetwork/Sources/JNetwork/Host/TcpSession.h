/*
	�ۼ��� : ������
	TcpClient�� �θ� Ŭ����
	
	2���� ������ �����Ѵ�.
	1. ���������� ������ ������ Ŭ���̾�Ʈ�� ���� �����Ѵ�. - TcpSession
	2. Ŭ���̾�Ʈ�� �������� �����Ѵ�.					 - TcpClient

	��� �Լ�, ������ ��� protected�� �ξ���.
	friend class�� ���� �༭�鸸 �����Ͽ� ������ �� �ֵ�����
*/

#pragma once

#include <JCore/Event.h>

#include <JNetwork/Buffer.h>
#include <JNetwork/Socket.h>
#include <JNetwork/Host/Server.h>
#include <JNetwork/Host/TcpServerEventListener.h>
#include <JNetwork/Packet.h>
#include <JNetwork/IOCP/IOCP.h>


namespace JNetwork {

class TcpSession
{
public:
	enum class State
	{
		Uninitialized	= 0,
		Initailized		= 1,
		AcceptWait		= 2,		// �������� ���� ���� ���
		ConnectWait		= 2,		// Ŭ�󿡼� ���� �õ� ��� - ConnectEx�� Ȱ���� ������ ���� �õ��ÿ� ����� ��?
		Accepted		= 3,		// �������� ���� ���� �Ϸ�
		Connected		= 3,		// Ŭ�󿡼� ���� �Ϸ�
		Disconnected	= 4
	};

public:
	IPv4EndPoint GetLocalEndPoint() const { return m_LocalEndPoint; }
	IPv4EndPoint GetRemoteEndPoint() const { return m_RemoteEndPoint; }
	SessionBuffer* GetReceiveBuffer() { return &m_ReceiveBuffer; }
	State GetState() const { return m_eState; }
	bool SendAsync(IPacket* packet);
protected:
	TcpSession(IOCP* iocp, TcpServerEventListener* listener = nullptr);
	virtual ~TcpSession();
	virtual bool Disconnect();
	
	bool ReceiveAsync();
	bool AcceptAsync(SOCKET hListeningSock, LPOVERLAPPED pOverlapped);
	bool CheckState(State state) { return m_eState == state; }
	
	TcpSocketv4 Socket() const { return m_ClientSocket; }

	virtual bool Initialize();
	virtual void AcceptWait();												// �̸� ������ TcpSession�� AcceptEx�� ȣ���ϱ� ������ ȣ��� �Լ� - ���� ���� ������
	virtual void ConnectWait() {};											// Ŭ���̾�Ʈ�� ������ ������ ��û�� ȣ��� �Լ�			(TcpSession������ ������� ����)
	virtual bool Accepted(SOCKET listeningSocket, Int32UL receivedBytes);	// ������ �ش缼���� ������ ������ ���������� ȣ��� �Լ�
	virtual void Connected() {};											// Ŭ���̾�Ʈ�� ������ ������� �� ���������� ȣ��� �Լ�	(TcpSession������ ������� ����)
	virtual void Received(Int32UL receivedBytes);							// ���� �Ǵ� Ŭ���̾�Ʈ�� �����͸� �������� �� ���������� ȣ��� �Լ�
	virtual void Sent(IPacket* sentPacket, Int32UL receivedBytes);								// ���� �Ǵ� Ŭ���̾�Ʈ�� �����͸� �۽����� �� ���������� ȣ��� �Լ�
private:
	TcpServerEventListener* m_pServerEventListener;							// TcpClient ���忡���� nullptr�� ������� ���� / �׷��� private���� ��
protected:
	State m_eState;
	SessionBuffer m_ReceiveBuffer;
	TcpSocketv4 m_ClientSocket;
	IOCP* m_pIocp;															// TcpClient ���忡���� ����/�Ҹ��� ������ϴ� ��ü������ TcpSession ���忡���� TcpServer�� IOCP�� �ܼ��� �����ϴ� �뵵�̴�.
	IPv4EndPoint m_RemoteEndPoint;
	IPv4EndPoint m_LocalEndPoint;
	
	friend class TcpServer;
	friend class TcpSessionContainer;
	friend class IOCPOverlappedAccept;
	friend class IOCPOverlappedConnect;
	friend class IOCPOverlappedReceive;
	friend class IOCPOverlappedSend;
};

} // namespace JNetwork