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

		Type ConnType;
		ClientConnectServerType_t ListenerType;
		SGSession* Session;
	};

	struct ConnectionResult : ResultBase, SGObjectPool<ConnectionResult>
	{
		bool Success;
		Int32U ErrorCode;
	};

	struct DisconnectionResult : ResultBase, SGObjectPool<DisconnectionResult>
	{
	};

	void processConnection(ConnectionResult* _pResult);
	void processDisconnection(DisconnectionResult* _pResult);
	void finalize();

public:
	void enqueueConnection(ClientConnectServerType_t _listenerType, SGSession* _pSession, bool _success,
	                       Int32U _errorCode);
	void enqueueDisconnection(ClientConnectServerType_t _listenerType, SGSession* _pSession);
	void initialize();
	void processConnections();

private:
	using ResultQueue = SGArrayQueue<ResultBase*>;

	SGActionFn<SGSession*> onConnected_[ClientConnectServerType::Max];
	SGActionFn<SGSession*, Int32U> onConnectFailed_[ClientConnectServerType::Max];
	SGActionFn<SGSession*> onDisconnected_[ClientConnectServerType::Max];

	SGNormalLock lock_;
	ResultQueue* connectionResultQueue_;
	ResultQueue* swapQueue_;
};
