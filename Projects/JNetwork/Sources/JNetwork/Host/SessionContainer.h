/*
	작성자 : 윤정도
*/


#pragma once

#include <JCore/Container/HashMap.h>
#include <JCore/Container/ArrayQueue.h>

#include <JCore/Sync/NormalLock.h>
#include <JNetwork/Host/Session.h>

NS_JNET_BEGIN

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
	bool m_bReuse;
	int m_iMaxConnection;
	JCore::NormalLock m_ContainerLock;
	JCore::HashMap<SOCKET, Session*> m_hAllSession;
	JCore::ArrayQueue<Session*> m_qReuseSessions;
};

NS_JNET_END