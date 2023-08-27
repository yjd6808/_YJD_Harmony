/*
 * 작성자: 윤정도
 * 생성일: 8/26/2023 5:00:05 PM
 * =====================
 *
 */


#pragma once

#include <JNetwork/Host/TcpSession.h>

#include <SteinsGate/Common/ComponentCollection.h>

class CommonSession : public JNetwork::TcpSession
{
public:
	CommonSession(
		JNetwork::TcpServer* server,
		const JNetwork::IOCPPtr& iocp,
		const JCore::MemoryPoolAbstractPtr& bufferAllocator,
		int recvBufferSize,
		int sendBufferSize
	);

	bool AddComponent(IComponent* component);
	bool HasComponent(int type);

	template <typename TComponent>
	TComponent* GetComponent(int type) const { return m_Components.Get<TComponent>(type); }

	void OnCreated() override;
	void OnConnected() override;
	void OnDisconnected() override;
private:
	ComponentCollection m_Components;
};

