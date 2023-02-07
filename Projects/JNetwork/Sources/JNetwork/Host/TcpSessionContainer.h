/*
	작성자 : 윤정도
*/


#pragma once

#include <JCore/Container/HashMap.h>
#include <JCore/Sync/NormalLock.h>

#include <JNetwork/Host/TcpSession.h>

NS_JNET_BEGIN

class TcpServer;
class TcpSessionContainer
{
public:
	TcpSessionContainer(int size);
	~TcpSessionContainer();

	int MaxConnection() const { return m_iMaxConnection; }
	bool AddSession(TcpSession* session);
	TcpSession* GetSession(SOCKET socket);
	void DisconnectAllSessions();
	void Clear();
	
private:
	int m_iMaxConnection;
	JCore::NormalLock m_ContainerLock;
	JCore::HashMap<SOCKET, TcpSession*> m_Container;

	friend class TcpServer;
};

NS_JNET_END