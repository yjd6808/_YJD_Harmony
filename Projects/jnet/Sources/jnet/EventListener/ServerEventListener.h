/*
	작성자 : 윤정도
	서버의 이벤트리스너

	사용법
	1. 자신만의 커스텀 리스너 클래스를 만든 후 이 인터페이스 클래스를 상속받아서 오버라이딩한 후 구현한다.
	2. 서버에 SetEventListener로 커스텀 리스너를 등록해준다.

	[주의사항]
	1. OnConnected에서 연결된 세션에게 먼저 패킷 송신 금지
	   서버의 Accepted 오버랩이 완전히 완료된 후에야 서버측에서 먼저 송신이 가능해지기 때문이다.
	   왠만하면 클라이언트의 OnConnected에서 패킷 송신을 먼저 하도록 할것
	   왜 Accepted 오버랩이 완전히 처리된 후에 서버측에서 먼저 송신이 가능해지는지 이유는 잘 모르겠다.
*/

#pragma once

#include "jnet/EventListener/SessionEventListener.h"

NS_JNET_BEGIN

class ICommand;
class IPacket;
struct RecvedPacket;
class TcpServer;
class Session;
class Server;

class ServerEventListener : public SessionEventListener
{
public:
	using FnStarted = jc::Action<>;
	using FnStartFailed = jc::Action<_u32>;
	using FnStopped = jc::Action<>;

	virtual ~ServerEventListener() = default;
	virtual void OnStarted() {}
	virtual void OnStartFailed(_u32 _errorCode) {}
	virtual void OnStopped() {}
	virtual void OnConnectFailed(Session* _pSession, _u32 _errorCode) {}

	void SetStartedCallback(const FnStarted& _fn) { fnStarted_ = _fn; }
	void SetStartFailedCallback(const FnStartFailed& _fn) { fnStartFailed_ = _fn; }
	void SetStoppedCallback(const FnStopped& _fn) { fnStopped_ = _fn; }
	void SetStartedCallback(FnStarted&& _fn) { fnStarted_ = std::move(_fn); }
	void SetStartFailedCallback(FnStartFailed&& _fn) { fnStartFailed_ = std::move(_fn); }
	void SetStoppedCallback(FnStopped&& _fn) { fnStopped_ = std::move(_fn); }
private:
	FnStarted fnStarted_; 
	FnStartFailed fnStartFailed_; 
	FnStopped fnStopped_; 
};

NS_END
