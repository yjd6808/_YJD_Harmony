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

ConnectionSynchronizer::ConnectionSynchronizer()
	: m_qConnectionResults(nullptr)
	, m_qSwap(nullptr)
{}

ConnectionSynchronizer::~ConnectionSynchronizer() {
	finalize();
}


void ConnectionSynchronizer::initialize() {
	m_qConnectionResults = dbg_new SGArrayQueue<ResultBase*>();
	m_qSwap = dbg_new SGArrayQueue<ResultBase*>();

	m_fnOnConnected[ClientConnectServerType::Auth] = C_AUTH::OnConnected;
	m_fnOnConnected[ClientConnectServerType::Lobby] = C_LOBBY::OnConnected;
	m_fnOnConnected[ClientConnectServerType::Game] = C_GAME::OnConnected;
	m_fnOnConnected[ClientConnectServerType::Chat] = C_CHAT::OnConnected;
	m_fnOnConnected[ClientConnectServerType::Area] = C_AREA::OnConnected;

	m_fnOnConnectFailed[ClientConnectServerType::Auth] = C_AUTH::OnConnectFailed;
	m_fnOnConnectFailed[ClientConnectServerType::Lobby] = C_LOBBY::OnConnectFailed;
	m_fnOnConnectFailed[ClientConnectServerType::Game] = C_GAME::OnConnectFailed;
	m_fnOnConnectFailed[ClientConnectServerType::Chat] = C_CHAT::OnConnectFailed;
	m_fnOnConnectFailed[ClientConnectServerType::Area] = C_AREA::OnConnectFailed;

	m_fnOnDisconnected[ClientConnectServerType::Auth] = C_AUTH::OnDisconnected;
	m_fnOnDisconnected[ClientConnectServerType::Lobby] = C_LOBBY::OnDisconnected;
	m_fnOnDisconnected[ClientConnectServerType::Game] = C_GAME::OnDisconnected;
	m_fnOnDisconnected[ClientConnectServerType::Chat] = C_CHAT::OnDisconnected;
	m_fnOnDisconnected[ClientConnectServerType::Area] = C_AREA::OnDisconnected;
}

void ConnectionSynchronizer::processConnections() {
	ResultQueue* pQueue;
	{
		LOCK_GUARD(m_Lock);

		if (m_qConnectionResults->Size() == 0) {
			return;
		}

		pQueue = m_qConnectionResults;
		m_qConnectionResults = m_qSwap;
		m_qSwap = pQueue;
		
	}

	while (!pQueue->IsEmpty()) {
		const ResultBase* pResult = pQueue->Front();

		switch (pResult->ConnType) {
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

void ConnectionSynchronizer::processConnection(ConnectionResult* result) {
	if (result->Success) {
		m_fnOnConnected[result->ListenerType](result->Session);
	} else {
		m_fnOnConnectFailed[result->ListenerType](result->Session, result->ErrorCode);
	}
}

void ConnectionSynchronizer::processDisconnection(DisconnectionResult* result) {
	m_fnOnDisconnected[result->ListenerType](result->Session);
}

void ConnectionSynchronizer::finalize() {
	ResultQueue* pQueue;

	// 미처리 데이터 삭제
	{
		pQueue = m_qSwap;
		while (!pQueue->IsEmpty()) {
			delete pQueue->Front();
			pQueue->Dequeue();
		}
	}
	{
		pQueue = m_qConnectionResults;
		while (!pQueue->IsEmpty()) {
			delete pQueue->Front();
			pQueue->Dequeue();
		}
	}

	DeleteSafe(m_qConnectionResults);
	DeleteSafe(m_qSwap);

	ConnectionResult::FreeAllObjects();
	DisconnectionResult::FreeAllObjects();
}

void ConnectionSynchronizer::enqueueConnection(ClientConnectServerType_t listenerType, SGSession* session, bool success, Int32U errorCode) {
	LOCK_GUARD(m_Lock);
	ConnectionResult* pResult = dbg_new ConnectionResult;
	pResult->ConnType = eConnection;
	pResult->Success = success;
	pResult->ListenerType = listenerType;
	pResult->ErrorCode = success ? 0 : errorCode;
	pResult->Session = session;
	m_qConnectionResults->Enqueue(pResult);
}

void ConnectionSynchronizer::enqueueDisconnection(ClientConnectServerType_t listenerType, SGSession* session) {
	LOCK_GUARD(m_Lock);
	DisconnectionResult* pResult = dbg_new DisconnectionResult;
	pResult->ConnType = eDisconnection;
	pResult->ListenerType = listenerType;
	pResult->Session = session;
	m_qConnectionResults->Enqueue(pResult);
}

