/*
 * 작성자: 윤정도
 * 생성일: 5/10/2023 2:51:45 AM
 * =====================
 *
 */

#include "Core.h"
#include "GameCoreHeader.h"
#include "ConnectionSynchronizer.h"

#include <sgcl/C_AUTH.h>
#include <sgcl/C_LOBBY.h>
#include <sgcl/C_GAME.h>
#include <sgcl/C_CHAT.h>
#include <sgcl/C_AREA.h>

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
	Finalize();
}

//////////////////////////////////////////////////////////////////////////////////////////
void ConnectionSynchronizer::Initialize()
{
	connectionResultQueue_ = dbg_new jc::ArrayQueue<ResultBase*>();
	swapQueue_ = dbg_new jc::ArrayQueue<ResultBase*>();

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
void ConnectionSynchronizer::ProcessConnections()
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

		switch (pResult->connType_)
		{
		case eConnection:
			{
				ConnectionResult* pConnResult = (ConnectionResult*)pResult;
				ProcessConnection(pConnResult);
				break;
			}
		case eDisconnection:
			{
				DisconnectionResult* pDisconnectionResult = (DisconnectionResult*)pResult;
				ProcessDisconnection(pDisconnectionResult);
				break;
			}
		}

		pQueue->Dequeue();
		delete pResult;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ConnectionSynchronizer::ProcessConnection(ConnectionResult* _pResult)
{
	if (_pResult->success_)
	{
		onConnected_[_pResult->listenerType_](_pResult->pSession_);
	}
	else
	{
		onConnectFailed_[_pResult->listenerType_](_pResult->pSession_, _pResult->errorCode_);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ConnectionSynchronizer::ProcessDisconnection(DisconnectionResult* _pResult)
{
	onDisconnected_[_pResult->listenerType_](_pResult->pSession_);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ConnectionSynchronizer::Finalize()
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
void ConnectionSynchronizer::EnqueueConnection(ClientConnectServerType_t _listenerType, jnet::Session* _pSession,
                                               bool _success, Int32U _errorCode)
{
	JCORE_LOCK_GUARD(lock_);
	ConnectionResult* pResult = dbg_new ConnectionResult;
	pResult->connType_ = eConnection;
	pResult->success_ = _success;
	pResult->listenerType_ = _listenerType;
	pResult->errorCode_ = _success ? 0 : _errorCode;
	pResult->pSession_ = _pSession;
	connectionResultQueue_->Enqueue(pResult);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ConnectionSynchronizer::EnqueueDisconnection(ClientConnectServerType_t _listenerType, jnet::Session* _pSession)
{
	JCORE_LOCK_GUARD(lock_);
	DisconnectionResult* pResult = dbg_new DisconnectionResult;
	pResult->connType_ = eDisconnection;
	pResult->listenerType_ = _listenerType;
	pResult->pSession_ = _pSession;
	connectionResultQueue_->Enqueue(pResult);
}
