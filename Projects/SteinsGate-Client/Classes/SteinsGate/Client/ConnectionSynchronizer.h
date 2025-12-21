/*
 * 작성자: 윤정도
 * 생성일: 5/10/2023 2:51:33 AM
 * =====================
 */


#pragma once

#include <SteinsGate/Client/Tutturu.h>


class ConnectionSynchronizer final : public SGSingletonPointer<ConnectionSynchronizer>
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
		SGSession* pSession_;
	};

	struct ConnectionResult : ResultBase, SGObjectPool<ConnectionResult>
	{
		bool success_;
		Int32U errorCode_;
	};

	struct DisconnectionResult : ResultBase, SGObjectPool<DisconnectionResult>
	{
	};

	void ProcessConnection(ConnectionResult* _pResult);
	void ProcessDisconnection(DisconnectionResult* _pResult);
	void Finalize();

public:
	void EnqueueConnection(ClientConnectServerType_t _listenerType, SGSession* _pSession, bool _success, Int32U _errorCode);
	void EnqueueDisconnection(ClientConnectServerType_t _listenerType, SGSession* _pSession);
	void Initialize();
	void ProcessConnections();

private:
	using ResultQueue = SGArrayQueue<ResultBase*>;

	SGActionFn<SGSession*> onConnected_[ClientConnectServerType::Max];
	SGActionFn<SGSession*, Int32U> onConnectFailed_[ClientConnectServerType::Max];
	SGActionFn<SGSession*> onDisconnected_[ClientConnectServerType::Max];

	SGNormalLock lock_;
	ResultQueue* connectionResultQueue_;
	ResultQueue* swapQueue_;
};
