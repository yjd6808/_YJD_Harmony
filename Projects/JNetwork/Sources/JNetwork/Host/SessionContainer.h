/*
	작성자 : 윤정도
*/


#pragma once

#include <JCore/Container/HashMap.h>
#include <JCore/Sync/NormalLock.h>

#include <JNetwork/Host/Session.h>

NS_JNET_BEGIN

class TcpServer;
class SessionContainer
{
public:
	SessionContainer(int size);
	~SessionContainer();

	int MaxConnection() const { return m_iMaxConnection; }
	bool AddSession(Session* session);
	Session* GetSession(SOCKET socket);
	void DisconnectAllSessions();
	void Clear();
	
private:
	int m_iMaxConnection;
	JCore::NormalLock m_ContainerLock;
	JCore::HashMap<SOCKET, Session*> m_Container;

	friend class TcpServer;
};

NS_JNET_END