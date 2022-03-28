/*
	작성자 : 윤정도
	서버의 이벤트리스너

	사용법
	1. 자신만의 커스텀 리스너 클래스를 만든 후 이 인터페이스 클래스를 상속받아서 오버라이딩한 후 구현한다.
	2. 서버에 SetEventListener로 커스텀 리스너를 등록해준다.
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
	virtual void OnPaused() = 0;		// TCP 서버의 IOCP가 관리중인 모든 쓰레드가 일시정지가 될 때 호출된다.
	virtual void OnResume() = 0;		// TCP 서버의 IOCP가 관리중인 모든 쓰레드가 모두 시작된 후 호출된다.
	virtual void OnStopped() = 0;		// TCP 서버를 종료한 후 호출된다.

	friend class TcpSession;
	friend class TcpServer;
};

} // namespace JNetwork