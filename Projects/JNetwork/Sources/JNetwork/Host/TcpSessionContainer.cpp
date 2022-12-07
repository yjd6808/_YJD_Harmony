/*
 * 작성자 : 윤정도
 */

#include <JNetwork/Network.h>
#include <JNetwork/Host/TcpSessionContainer.h>

using namespace JCore;

namespace JNetwork {

TcpSessionContainer::TcpSessionContainer(int maxSize) : 
	m_iMaxConnection(maxSize), 
	m_Container(maxSize) 
{
}

TcpSessionContainer::~TcpSessionContainer() = default;

bool TcpSessionContainer::AddSession(TcpSession* session) {
	NormalLockGuard guard(m_ContainerLock);

	if (m_Container.Size() >= m_iMaxConnection) {
		return false;
	}

	if (m_Container.Exist(session->Socket().Handle())) {
		return false;
	}

	m_Container.Insert(session->Socket().Handle(), session);
	return true;
}

TcpSession* TcpSessionContainer::GetSession(SOCKET socket) {
	NormalLockGuard guard(m_ContainerLock);
	if (!m_Container.Exist(socket))
		return nullptr;

	return m_Container[socket];
}

void TcpSessionContainer::DisconnectAllSessions() {
	NormalLockGuard guard(m_ContainerLock);
	m_Container.Values().Extension().ForEach([](TcpSession* session) {
		session->Disconnect();
	});
}

void TcpSessionContainer::Clear() {
	NormalLockGuard guard(m_ContainerLock);

	m_Container.Values().Extension().ForEach([](TcpSession* session) {
		delete session;
	});
	m_Container.Clear();
}



}