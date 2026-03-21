/*
 * 작성자: 윤정도
 * 생성일: 8/26/2023 5:00:05 PM
 * =====================
 *
 */


#pragma once

#include "jnet/Host/TcpSession.h"

#include "sgs/ComponentCollection.h"

NS_SG_BEGIN

class NetSession : public jnet::TcpSession
{
public:
	NetSession(
		jnet::TcpServer* _pServer,
		const jnet::IOCPPtr& _pIocp,
		const jc::MemoryPoolAbstractPtr& _pBufferAllocator,
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

NS_END
