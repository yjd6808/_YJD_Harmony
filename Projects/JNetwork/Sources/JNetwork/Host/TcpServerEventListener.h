/*
	�ۼ��� : ������
	������ �̺�Ʈ������

	����
	1. �ڽŸ��� Ŀ���� ������ Ŭ������ ���� �� �� �������̽� Ŭ������ ��ӹ޾Ƽ� �������̵��� �� �����Ѵ�.
	2. ������ SetEventListener�� Ŀ���� �����ʸ� ������ش�.
*/

#pragma once

#include <JCore/Type.h>

namespace JNetwork {

struct IPacket;
class TcpServer;
class TcpSession;
class TcpServerEventListener
{
protected:
	virtual void OnStarted() = 0;
	virtual void OnConnected(TcpSession* connectedSession) = 0;
	virtual void OnDisconnected(TcpSession* disconnetedSession) = 0;
	virtual void OnSent(TcpSession* sender, IPacket* sentPacket, Int32UL sentBytes) = 0;
	virtual void OnReceived(TcpSession* receiver, Int32UL receivedBytes) = 0;
	virtual void OnPaused() = 0;		// TCP ������ IOCP�� �������� ��� �����尡 �Ͻ������� �� �� ȣ��ȴ�.
	virtual void OnResume() = 0;		// TCP ������ IOCP�� �������� ��� �����尡 ��� ���۵� �� ȣ��ȴ�.
	virtual void OnStopped() = 0;		// TCP ������ ������ �� ȣ��ȴ�.

	friend class TcpSession;
	friend class TcpServer;
};

} // namespace JNetwork