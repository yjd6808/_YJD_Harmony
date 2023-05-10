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
	~ConnectionSynchronizer() override;

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

	void processConnection(ConnectionResult* result);
	void processDisconnection(DisconnectionResult* result);
	void finalize();
public:
	void enqueueConnection(ClientConnectServerType_t listenerType, SGSession* session, bool success, Int32U errorCode);
	void enqueueDisconnection(ClientConnectServerType_t listenerType, SGSession* session);
	void initialize();
	void processConnections();
private:
	using ResultQueue = SGArrayQueue<ResultBase*>;
	SGActionFn<SGSession*> m_fnOnConnected[ClientConnectServerType::Max];
	SGActionFn<SGSession*, Int32U> m_fnOnConnectFailed[ClientConnectServerType::Max];
	SGActionFn<SGSession*> m_fnOnDisconnected[ClientConnectServerType::Max];

	SGNormalLock m_Lock;
	ResultQueue* m_qConnectionResults;
	ResultQueue* m_qSwap;
};



