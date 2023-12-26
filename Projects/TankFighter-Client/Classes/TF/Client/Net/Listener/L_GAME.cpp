/*
 * 작성자: 윤정도
 * 생성일: 10/10/2023 5:06:40 PM
 * =====================
 *
 */


#include "Pch.h"
#include "L_GAME.h"

#include <TF/Common/Command.h>
#include <TF/Client/Net/Connection/C_GAME.h>

USING_NS_CC;
USING_NS_JNET;

L_GAME::L_GAME()
	: m_pCommandQueue(dbg_new CommandQueue)
{}

L_GAME::~L_GAME() {
	ClearCommandQueue();
	JCORE_DELETE_SAFE(m_pCommandQueue);
}

void L_GAME::OnConnected(Session* session) {
	Director::getInstance()->getScheduler()->performFunctionInCocosThread(C_GAME::ConnectedSynchronized);
}

void L_GAME::OnConnectFailed(Session* session, Int32U errorCode) {
	Director::getInstance()->getScheduler()->performFunctionInCocosThread([=] {C_GAME::ConnectFailedSynchronized(errorCode); });
}

void L_GAME::OnDisconnected(Session* session) {
	Director::getInstance()->getScheduler()->performFunctionInCocosThread(C_GAME::DisconnectedSynchronized);
}

void L_GAME::OnSent(Session* session, IPacket* sentPacket, Int32UL sentBytes) {
	CommandPacket* pPacket = dynamic_cast<CommandPacket*>(sentPacket);

	if (pPacket == nullptr) {
		DebugAssert(false);
		return;
	}

	pPacket->ForEach([](ICommand* cmd) {
		Cmd_t uiCmd = cmd->GetCommand();
		if (Core::FilteredCommandSet.Exist(uiCmd)) return;
		_LogInfo_("%s(%d) 송신", Core::CommandNameMap.Get(uiCmd), uiCmd);
	});
}

void L_GAME::OnReceived(Session* session, ICommand* recvCmd) {
	ICommand* pClone = recvCmd->Clone();
	{
		JCORE_LOCK_GUARD(m_Lock);
		m_pCommandQueue->Enqueue(pClone);
	}

	const Cmd_t uiCmdId = recvCmd->GetCommand();
	if (Core::FilteredCommandSet.Exist(uiCmdId)) return;
	_LogDebug_("%s(%d) 커맨드 수신", Core::CommandNameMap.Get(uiCmdId), uiCmdId);
}

void L_GAME::OnReceived(Session* session, RecvedCommandPacket* recvPacket) {
}

void L_GAME::ClearCommandQueue() {
	JCORE_LOCK_GUARD(m_Lock);
	while (!m_pCommandQueue->IsEmpty()) {
		ICommand* pCmd = m_pCommandQueue->Front();
		m_pCommandQueue->Dequeue();
		delete[](char*)pCmd;
	}
}

bool L_GAME::SwapCommandQueue(JCORE_REF_OUT CommandQueue** target) {
	JCORE_LOCK_GUARD(m_Lock);
	JCORE_SWAP(*target, m_pCommandQueue, CommandQueue*);
	return true;
}
