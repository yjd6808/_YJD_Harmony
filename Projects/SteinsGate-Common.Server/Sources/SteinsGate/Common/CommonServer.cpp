/*
 * 작성자: 윤정도
 * 생성일: 4/21/2023 9:02:24 PM
 * =====================
 *
 */


#include "Server.h"
#include "ServerCoreHeader.h"
#include "CommonServer.h"

USING_NS_JC;
USING_NS_JNET;

CommonServer::CommonServer(
	const IOCPPtr& iocp, 
	const MemoryPoolAbstractPtr& bufferAllocator,
	ServerEventListener* eventListener, 
	int maxConn, 
	int sessionRecvBufferSize,
	int sessionSendBufferSize)
	: TcpServer(iocp, bufferAllocator, eventListener, maxConn, sessionRecvBufferSize, sessionSendBufferSize)
	, m_eBootState(ServerBootState::Initialized)
{}


void CommonServer::ProcessLoop(int sleepMs) {

	if (m_eBootState == ServerBootState::Launched) {
		return;
	}

	OnLoop(sleepMs);
}




bool CommonServer::ProcessOrder(CenterOrder_t order) {
	const ServerInfo info = GetServerInfo();
	const int eState = m_eBootState;

	switch (order) {
	case CenterOrder::LaunchServer:
		if (eState != ServerBootState::Stopped && eState != ServerBootState::Initialized) {
			return false;
		}

		m_eBootState = ServerBootState::Launching;
		if (Start(info.BindEndPoint)) {
			m_eBootState = ServerBootState::Launched;
			return true;
		}

		_NetLogError_("서버 실행을 실패했습니다.");
		Initialize();
		m_eBootState = ServerBootState::Initialized;
		return false;
	case CenterOrder::StopServer:
		if (m_eBootState != ServerBootState::Launched)
			return false;

		m_eBootState = ServerBootState::Stopping;
		Stop();
		Initialize();
		m_eBootState = ServerBootState::Stopped;
		return false;
	case CenterOrder::RebootServer:
		if (eState == ServerBootState::Stopping || eState == ServerBootState::Launching) {
			return false;
		}

		// 이미 정지중인 경우
		if (eState == ServerBootState::Stopped || eState == ServerBootState::Initialized) {
			return ProcessOrder(CenterOrder::LaunchServer);
		}

		// 실행중인 경우: 정지 후 실행
		if (eState == ServerBootState::Launched) {
			return ProcessOrder(CenterOrder::StopServer) && ProcessOrder(CenterOrder::LaunchServer);
		}

		return false;
	}

	return false;
}

