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


struct ICommand;
struct ISendPacket;
class TcpServer;
class TcpSession;
class TcpServerEventListener
{
public:
	TcpServerEventListener() = default;
	virtual ~TcpServerEventListener() = default;
protected:
	virtual void OnStarted() = 0;
	virtual void OnConnected(TcpSession* connectedSession) = 0;
	virtual void OnDisconnected(TcpSession* disconnetedSession) = 0;
	virtual void OnSent(TcpSession* sender, ISendPacket* sentPacket, Int32UL sentBytes) = 0;
	virtual void OnReceived(TcpSession* receiver, ICommand* cmd) = 0;
	virtual void OnStopped() = 0;		// TCP 서버를 종료한 후 호출된다.

	friend class TcpSession;
	friend class TcpServer;
};

} // namespace JNetwork