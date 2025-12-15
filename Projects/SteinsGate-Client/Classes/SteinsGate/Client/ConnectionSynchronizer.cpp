/*
 * 작성자: 윤정도
 * 생성일: 5/10/2023 2:51:45 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "ConnectionSynchronizer.h"

#include <SteinsGate/Client/C_AUTH.h>
#include <SteinsGate/Client/C_LOBBY.h>
#include <SteinsGate/Client/C_GAME.h>
#include <SteinsGate/Client/C_CHAT.h>
#include <SteinsGate/Client/C_AREA.h>

USING_NS_JC;
USING_NS_CC;
USING_NS_JNET;

//////////////////////////////////////////////////////////////////////////////////////////
ConnectionSynchronizer::ConnectionSynchronizer()
: connectionResultQueue_(nullptr)
, swapQueue_(nullptr)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
ConnectionSynchronizer::~ConnectionSynchronizer()
{
	finalize();
}

//////////////////////////////////////////////////////////////////////////////////////////
void ConnectionSynchronizer::initialize()
{
	connectionResultQueue_ = dbg_new SGArrayQueue<ResultBase*>();
	swapQueue_ = dbg_new SGArrayQueue<ResultBase*>();

	onConnected_[ClientConnectServerType::Auth] = C_AUTH::OnConnected;
	onConnected_[ClientConnectServerType::Lobby] = C_LOBBY::OnConnected;
	onConnected_[ClientConnectServerType::Logic] = C_GAME::OnConnected;
	onConnected_[ClientConnectServerType::Chat] = C_CHAT::OnConnected;
	onConnected_[ClientConnectServerType::Area] = C_AREA::OnConnected;

	onConnectFailed_[ClientConnectServerType::Auth] = C_AUTH::OnConnectFailed;
	onConnectFailed_[ClientConnectServerType::Lobby] = C_LOBBY::OnConnectFailed;
	onConnectFailed_[ClientConnectServerType::Logic] = C_GAME::OnConnectFailed;
	onConnectFailed_[ClientConnectServerType::Chat] = C_CHAT::OnConnectFailed;
	onConnectFailed_[ClientConnectServerType::Area] = C_AREA::OnConnectFailed;

	onDisconnected_[ClientConnectServerType::Auth] = C_AUTH::OnDisconnected;
	onDisconnected_[ClientConnectServerType::Lobby] = C_LOBBY::OnDisconnected;
	onDisconnected_[ClientConnectServerType::Logic] = C_GAME::OnDisconnected;
	onDisconnected_[ClientConnectServerType::Chat] = C_CHAT::OnDisconnected;
	onDisconnected_[ClientConnectServerType::Area] = C_AREA::OnDisconnected;
}

//////////////////////////////////////////////////////////////////////////////////////////
void ConnectionSynchronizer::processConnections()
{
	ResultQueue* pQueue;
	{
		JCORE_LOCK_GUARD(lock_);

		if (connectionResultQueue_->Size() == 0)
		{
			return;
		}

		pQueue = connectionResultQueue_;
		connectionResultQueue_ = swapQueue_;
		swapQueue_ = pQueue;
	}

	while (!pQueue->IsEmpty())
	{
		const ResultBase* pResult = pQueue->Front();

		switch (pResult->ConnType)
		{
		case eConnection:
			{
				ConnectionResult* pConnResult = (ConnectionResult*)pResult;
				processConnection(pConnResult);
				break;
			}
		case eDisconnection:
			{
				DisconnectionResult* pDisconnectionResult = (DisconnectionResult*)pResult;
				processDisconnection(pDisconnectionResult);
				break;
			}
		}

		pQueue->Dequeue();
		delete pResult;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ConnectionSynchronizer::processConnection(ConnectionResult* _pResult)
{
	if (_pResult->Success)
	{
		onConnected_[_pResult->ListenerType](_pResult->Session);
	}
	else
	{
		onConnectFailed_[_pResult->ListenerType](_pResult->Session, _pResult->ErrorCode);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ConnectionSynchronizer::processDisconnection(DisconnectionResult* _pResult)
{
	onDisconnected_[_pResult->ListenerType](_pResult->Session);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ConnectionSynchronizer::finalize()
{
	ResultQueue* pQueue;

	// 미처리 데이터 삭제
	{
		pQueue = swapQueue_;
		while (!pQueue->IsEmpty())
		{
			delete pQueue->Front();
			pQueue->Dequeue();
		}
	}
	{
		pQueue = connectionResultQueue_;
		while (!pQueue->IsEmpty())
		{
			delete pQueue->Front();
			pQueue->Dequeue();
		}
	}

	JCORE_DELETE_SAFE(connectionResultQueue_);
	JCORE_DELETE_SAFE(swapQueue_);

	ConnectionResult::FreeAllObjects();
	DisconnectionResult::FreeAllObjects();
}

//////////////////////////////////////////////////////////////////////////////////////////
void ConnectionSynchronizer::enqueueConnection(ClientConnectServerType_t _listenerType, SGSession* _pSession,
                                               bool _success, Int32U _errorCode)
{
	JCORE_LOCK_GUARD(lock_);
	ConnectionResult* pResult = dbg_new ConnectionResult;
	pResult->ConnType = eConnection;
	pResult->Success = _success;
	pResult->ListenerType = _listenerType;
	pResult->ErrorCode = _success ? 0 : _errorCode;
	pResult->Session = _pSession;
	connectionResultQueue_->Enqueue(pResult);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ConnectionSynchronizer::enqueueDisconnection(ClientConnectServerType_t _listenerType, SGSession* _pSession)
{
	JCORE_LOCK_GUARD(lock_);
	DisconnectionResult* pResult = dbg_new DisconnectionResult;
	pResult->ConnType = eDisconnection;
	pResult->ListenerType = _listenerType;
	pResult->Session = _pSession;
	connectionResultQueue_->Enqueue(pResult);
}
