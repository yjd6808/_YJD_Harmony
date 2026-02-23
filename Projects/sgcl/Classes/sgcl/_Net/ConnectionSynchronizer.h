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
		ServerType_t listenerType_;
		jnet::Session* pSession_;
	};

	struct ConnectionResult : ResultBase, jc::ObjectPool<ConnectionResult>
	{
		bool success_;
		_u32 errorCode_;
	};

	struct DisconnectionResult : ResultBase, jc::ObjectPool<DisconnectionResult>
	{
	};

	void ProcessConnection(ConnectionResult* _pResult);
	void ProcessDisconnection(DisconnectionResult* _pResult);
	void Finalize();

public:
	void EnqueueConnection(ServerType_t _listenerType, jnet::Session* _pSession, bool _success, _u32 _errorCode);
	void EnqueueDisconnection(ServerType_t _listenerType, jnet::Session* _pSession);
	void Initialize();
	void ProcessConnections();

private:
	using ResultQueue = jc::ArrayQueue<ResultBase*>;

	jc::Action<jnet::Session*> onConnected_[ServerType::Max];
	jc::Action<jnet::Session*, _u32> onConnectFailed_[ServerType::Max];
	jc::Action<jnet::Session*> onDisconnected_[ServerType::Max];

	jc::NormalLock lock_;
	ResultQueue* connectionResultQueue_;
	ResultQueue* swapQueue_;
};
