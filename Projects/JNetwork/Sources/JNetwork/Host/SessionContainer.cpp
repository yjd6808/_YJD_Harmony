/*
 * 작성자 : 윤정도
 */

#include <JNetwork/Network.h>
#include <JNetwork/Host/SessionContainer.h>

USING_NS_JC;

NS_JNET_BEGIN

SessionContainer::SessionContainer(int maxSize)
	: m_iMaxConnection(maxSize)
	, m_hAllSession(maxSize)
{}

SessionContainer::~SessionContainer() = default;

bool SessionContainer::AddSession(Session* session) {
	JCORE_LOCK_GUARD(m_ContainerLock);

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
	JCORE_LOCK_GUARD(m_ContainerLock);

	if (!m_hAllSession.Exist(socket))
		return nullptr;

	return m_hAllSession[socket];
}

void SessionContainer::DisconnectAllSessions() {
	JCORE_LOCK_GUARD(m_ContainerLock);
	m_hAllSession.ForEachValue([](Session* session) {
		session->Disconnect();
		session->WaitForZeroPending();
	});
}

void SessionContainer::Clear() {
	JCORE_LOCK_GUARD(m_ContainerLock);

	m_hAllSession.ForEachValue([](Session* session) {
		delete session;
	});
	m_hAllSession.Clear();
}

void SessionContainer::ForEach(Action<Session*> fnForEach) {
	JCORE_LOCK_GUARD(m_ContainerLock);
	m_hAllSession.ForEachValue([&fnForEach](Session* session) {
		fnForEach(session);
	});
}

void SessionContainer::ForEachConnected(JCore::Action<Session*> fnForEach) {
	JCORE_LOCK_GUARD(m_ContainerLock);
	m_hAllSession.ForEachValue([&fnForEach](Session* session) {
		if (session->GetState() == Host::eConnected)
			fnForEach(session);
	});
}

NS_JNET_END
