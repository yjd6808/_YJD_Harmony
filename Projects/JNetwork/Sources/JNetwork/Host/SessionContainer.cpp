/*
 * 작성자 : 윤정도
 */

#include <JNetwork/Network.h>
#include <JNetwork/Host/SessionContainer.h>

USING_NS_JC;

NS_JNET_BEGIN

SessionContainer::SessionContainer(int maxSize) : 
	m_iMaxConnection(maxSize), 
	m_Container(maxSize) 
{
}

SessionContainer::~SessionContainer() = default;

bool SessionContainer::AddSession(Session* session) {
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

Session* SessionContainer::GetSession(SOCKET socket) {
	NormalLockGuard guard(m_ContainerLock);
	if (!m_Container.Exist(socket))
		return nullptr;

	return m_Container[socket];
}

void SessionContainer::DisconnectAllSessions() {
	NormalLockGuard guard(m_ContainerLock);
	m_Container.Values().Extension().ForEach([](Session* session) {
		session->Disconnect();
	});
}

void SessionContainer::Clear() {
	NormalLockGuard guard(m_ContainerLock);

	m_Container.Values().Extension().ForEach([](Session* session) {
		delete session;
	});
	m_Container.Clear();
}



}