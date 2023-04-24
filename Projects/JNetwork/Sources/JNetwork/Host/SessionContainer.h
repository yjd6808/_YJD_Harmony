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
	using Iterator = JCore::SharedPtr<JCore::HashMap<SOCKET, Session*>::ValueCollectionIterator>;

	SessionContainer(int size);
	~SessionContainer();

	int MaxConnection() const { return m_iMaxConnection; }
	bool AddSession(Session* session);
	Session* GetSession(SOCKET socket);
	void DisconnectAllSessions();
	void Clear();

	Iterator Begin() { return m_hAllSession.Values().Begin(); }
	Iterator End() { return m_hAllSession.Values().End(); }
	void ForEach(JCore::Action<Session*> fnForEach);
	void ForEachConnected(JCore::Action<Session*> fnForEach);
private:
	int m_iMaxConnection;
	JCore::NormalLock m_ContainerLock;
	JCore::HashMap<SOCKET, Session*> m_hAllSession;
};

NS_JNET_END