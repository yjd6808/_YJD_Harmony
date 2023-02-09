#pragma once

#include <JNetwork/Host/TcpClientEventListener.h>
#include <JCore/Container/ArrayQueue.h>
#include <JCore/Sync/NormalRwLock.h>

class GameClientEventListener : public JNetwork::TcpClientEventListener
{
protected:
	void OnConnected() override;
	void OnDisconnected() override;
	void OnSent(JNetwork::ISendPacket* sentPacket, Int32UL sentBytes) override;
	void OnReceived(JNetwork::ICommand* cmd) override;

	void SynchronizedOnConnected();
	void SynchronizedOnReceived();
	void SynchronizedOnDisconnected();
private:
	JCore::NormalLock m_CommandQueueMtx;
	JCore::ArrayQueue<char*> m_CommandQueue;
	//JCore::Atomic<int> m_iPendingCommandCount;
};

