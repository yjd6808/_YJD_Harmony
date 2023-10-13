/*
 * 작성자: 윤정도
 * 생성일: 10/13/2023 10:41:49 AM
 * =====================
 *
 */

#include "Pch.h"
#include "NetCore.h"

#include <TF/Client/Net/Listener/L_GAME.h>
#include <TF/Client/Net/Send/S_GAME.h>

USING_NS_JC;
USING_NS_JNET;

NetCore::NetCore()
	: m_pListenerGame(nullptr)
	, m_pCommandQueue(nullptr)
{}
NetCore::~NetCore() {
}


void NetCore::Initialize() {
	initializeBuffer();
	initializeIOCP();
	initializeHost();
	initializeParser();
}

void NetCore::initializeBuffer() {
	CreateBufferPool({});
}

void NetCore::initializeIOCP() {
	CreateIocp(1);
	RunIocp();
}

void NetCore::initializeHost() {
	m_spGameClient = MakeShared<GameClient>(m_spIOCP, m_spBufferPool);
	m_spGameClient->SetEventListener(m_pListenerGame = dbg_new L_GAME);
	
	AddHost(0, m_spGameClient);

	if (m_spGameClient->ConnectAsync(JNetwork::IPv4EndPoint::Parse(Const::Net::EndPoint::Connect))) {
		S_GAME::SetInformation(m_spGameClient.GetPtr(), SendStrategy::SendAsync);
	}
}

void NetCore::initializeParser() {
	
}


void NetCore::update(const TimeSpan& elpased) {
	processCommands();
}

void NetCore::processCommands() {

	if (m_pListenerGame == nullptr || m_spGameClient == nullptr) {
		return;
	}

	m_pListenerGame->SwapCommandQueue(&m_pCommandQueue);

	while (!m_pCommandQueue->IsEmpty()) {
		ICommand* pCmd = m_pCommandQueue->Front();
		m_Parser.RunCommand(m_spGameClient.GetPtr(), pCmd);
		delete[](char*)pCmd;
	}
}
