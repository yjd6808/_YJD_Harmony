/*
 * 작성자: 윤정도
 * 생성일: 8/26/2023 5:00:15 PM
 * =====================
 *
 */

#include "Server.h"
#include "ServerCoreHeader.h"
#include "CommonSession.h"

USING_NS_JC;
USING_NS_JNET;

CommonSession::CommonSession(
	TcpServer* server,
	const IOCPPtr& iocp,
	const JCore::MemoryPoolAbstractPtr& bufferAllocator,
	int recvBufferSize,
	int sendBufferSize) : TcpSession(server, iocp, bufferAllocator, recvBufferSize, sendBufferSize)
{}

bool CommonSession::AddComponent(IComponent* component) {
	return m_Components.Add(component);
}

bool CommonSession::HasComponent(int type) {
	return m_Components.Has(type);
}

void CommonSession::OnCreated() {

}

void CommonSession::OnConnected() {
	m_Components.Initialize();
	m_Components.OnConnected();
}

void CommonSession::OnDisconnected() {
	m_Components.OnDisconnected();
}

