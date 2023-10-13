/*
 * 작성자: 윤정도
 * 생성일: 10/10/2023 5:06:40 PM
 * =====================
 *
 */


#include "Pch.h"
#include "L_GAME.h"

#include <TF/Client/Net/Connection/C_GAME.h>

USING_NS_CC;

L_GAME::L_GAME()
	: m_pCommandQueue(dbg_new CommandQueue)
{}

L_GAME::~L_GAME() {
	JCORE_DELETE_SAFE(m_pCommandQueue);
}

void L_GAME::OnConnected(JNetwork::Session* session) {
	Director::getInstance()->getScheduler()->performFunctionInCocosThread(C_GAME::ConnectedSynchronized);
}

void L_GAME::OnConnectFailed(JNetwork::Session* session, Int32U errorCode) {
	Director::getInstance()->getScheduler()->performFunctionInCocosThread([=] {C_GAME::ConnectFailedSynchronized(errorCode); });
}

void L_GAME::OnDisconnected(JNetwork::Session* session) {
	Director::getInstance()->getScheduler()->performFunctionInCocosThread(C_GAME::DisconnectedSynchronized);
}

void L_GAME::OnSent(JNetwork::Session* session, JNetwork::ISendPacket* sentPacket, Int32UL sentBytes) {
}

void L_GAME::OnReceived(JNetwork::Session* session, JNetwork::ICommand* recvCmd) {
	JNetwork::ICommand* pClone = recvCmd->Clone();
	{
		JCORE_LOCK_GUARD(m_Lock);
		m_pCommandQueue->Enqueue(pClone);
	}

}

void L_GAME::OnReceived(JNetwork::Session* session, JNetwork::IRecvPacket* recvPacket) {
}

void L_GAME::SwapCommandQueue(JCORE_REF_OUT CommandQueue** target) {
	JCORE_LOCK_GUARD(m_Lock);
	JCORE_SWAP(*target, m_pCommandQueue, CommandQueue*);
}
