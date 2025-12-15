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
		JNetwork::TcpServer* _pServer,
		const JNetwork::IOCPPtr& _pIocp,
		const JCore::MemoryPoolAbstractPtr& _pBufferAllocator,
		int _recvBufferSize,
		int _sendBufferSize);

	bool AddComponent(IComponent* _pComponent);
	bool HasComponent(int _type);

	template <typename TComponent>
	TComponent* GetComponent(bool _addRef = false) const
	{
		return components_.Get<TComponent>(_addRef);
	}

	void OnCreated() override;
	void OnConnected() override;
	void OnDisconnected() override;

protected:
	ComponentCollection components_;
};
