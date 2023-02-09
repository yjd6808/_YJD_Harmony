/*
 * 작성자 : 윤정도
 */

#include <JNetwork/Network.h>
#include <JNetwork/Host/SessionContainer.h>

USING_NS_JC;

NS_JNET_BEGIN

SessionContainer::SessionContainer(int maxSize) : 
	m_iMaxConnection(maxSize), 
	m_hAllSession(maxSize) 
{
}

SessionContainer::~SessionContainer() = default;

bool SessionContainer::AddSession(Session* session) {
	NormalLockGuard guard(m_ContainerLock);

	if (m_hAllSession.Size() >= m_iMaxConnection) {
		return false;
	}

	if (m_hAllSession.Exist(session->SocketHandle())) {
		return false;
	}

	m_hAllSession.Insert(session->SocketHandle(), session);
	return true;
}

Session* SessionContainer::GetSession(SOCKET socket) {
	NormalLockGuard guard(m_ContainerLock);
	if (!m_hAllSession.Exist(socket))
		return nullptr;

	return m_hAllSession[socket];
}

void SessionContainer::DisconnectAllSessions() {
	NormalLockGuard guard(m_ContainerLock);
	m_hAllSession.Values().Extension().ForEach([](Session* session) {
		session->Disconnect();
		session->WaitForZeroPending();
	});
}

void SessionContainer::Clear() {
	NormalLockGuard guard(m_ContainerLock);

	m_hAllSession.Values().Extension().ForEach([](Session* session) {
		delete session;
	});
	m_hAllSession.Clear();
}



}