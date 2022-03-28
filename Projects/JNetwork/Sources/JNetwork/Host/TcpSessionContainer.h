/*
	작성자 : 윤정도
*/


#pragma once

#include <JCore/Container/HashMap.h>
#include <JCore/LockGuard.h>

#include <JNetwork/Host/TcpSession.h>

namespace JNetwork {

class TcpServer;
class TcpSessionContainer
{
public:
	TcpSessionContainer(int size);
	~TcpSessionContainer();

	int MaxConnection() const { return m_iMaxConnection; }
	bool AddSession(TcpSession* session);
	TcpSession* GetSession(SOCKET socket);
	void Clear();
private:
	int m_iMaxConnection;
	JCore::CriticalSectionMutex m_ContainerLock;
	JCore::HashMap<SOCKET, TcpSession*> m_Container;

	friend class TcpServer;
};

} // namespace JNetwork