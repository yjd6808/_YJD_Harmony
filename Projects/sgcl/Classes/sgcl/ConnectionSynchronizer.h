/*
 * 작성자: 윤정도
 * 생성일: 5/10/2023 2:51:33 AM
 * =====================
 */


#pragma once

#include <sgcl/Core.h>


class ConnectionSynchronizer final : public jc::SingletonPointer<ConnectionSynchronizer>
{
	friend class TSingleton;
	ConnectionSynchronizer();
	~ConnectionSynchronizer();

	enum Type
	{
		eConnection,
		eDisconnection
	};

	struct ResultBase
	{
		virtual ~ResultBase() = default;

		Type connType_;
		ClientConnectServerType_t listenerType_;
		jnet::Session* pSession_;
	};

	struct ConnectionResult : ResultBase, jc::ObjectPool<ConnectionResult>
	{
		bool success_;
		Int32U errorCode_;
	};

	struct DisconnectionResult : ResultBase, jc::ObjectPool<DisconnectionResult>
	{
	};

	void ProcessConnection(ConnectionResult* _pResult);
	void ProcessDisconnection(DisconnectionResult* _pResult);
	void Finalize();

public:
	void EnqueueConnection(ClientConnectServerType_t _listenerType, jnet::Session* _pSession, bool _success, Int32U _errorCode);
	void EnqueueDisconnection(ClientConnectServerType_t _listenerType, jnet::Session* _pSession);
	void Initialize();
	void ProcessConnections();

private:
	using ResultQueue = jc::ArrayQueue<ResultBase*>;

	jc::Action<jnet::Session*> onConnected_[ClientConnectServerType::Max];
	jc::Action<jnet::Session*, Int32U> onConnectFailed_[ClientConnectServerType::Max];
	jc::Action<jnet::Session*> onDisconnected_[ClientConnectServerType::Max];

	jc::NormalLock lock_;
	ResultQueue* connectionResultQueue_;
	ResultQueue* swapQueue_;
};
