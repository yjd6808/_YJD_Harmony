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
	CriticalSectionLockGuard guard(m_ContainerLock);

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
	CriticalSectionLockGuard guard(m_ContainerLock);
	if (!m_Container.Exist(socket))
		return nullptr;

	return m_Container[socket];
}

void TcpSessionContainer::Clear() {
	CriticalSectionLockGuard guard(m_ContainerLock);

	// �ϴ� ������ ���⼭ ��������.
	m_Container.Values().Extension().ForEach([](TcpSession* session) {
		session->Disconnect();
		delete session;
	});
	m_Container.Clear();
}



}