/*
 * 작성자: 윤정도
 * 생성일: 10/10/2023 5:04:38 PM
 * =====================
 *
 */


#pragma once

#include <JNetwork/EventListener/ClientEventListener.h>

#include <TF/Client/TypeDef.h>

class L_GAME : public JNetwork::ClientEventListener
{
public:
	using TLock = JCore::SpinLock;

	L_GAME();
	~L_GAME() override;
protected:
	void OnConnected(JNetwork::Session* session) override;
	void OnConnectFailed(JNetwork::Session* session, Int32U errorCode) override;
	void OnDisconnected(JNetwork::Session* session) override;
	void OnSent(JNetwork::Session* session, JNetwork::IPacket* sentPacket, Int32UL sentBytes) override;
	void OnReceived(JNetwork::Session* session, JNetwork::ICommand* recvCmd) override;
	void OnReceived(JNetwork::Session* session, JNetwork::RecvedCommandPacket* recvPacket) override;
public:
	void ClearCommandQueue();
	bool SwapCommandQueue(JCORE_REF_OUT CommandQueue** target);
private:
	TLock m_Lock;
	CommandQueue* m_pCommandQueue;	// 확장시 ThreadLocal로 두는게 효율적임
};
