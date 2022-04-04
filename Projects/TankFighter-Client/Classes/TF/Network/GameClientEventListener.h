#pragma once

#include <JNetwork/Host/TcpClientEventListener.h>
#include <JCore/Container/ArrayQueue.h>
#include <JCore/LockGuard.h>

class GameClientEventListener : public JNetwork::TcpClientEventListener
{
protected:
	void OnConnected() override;
	void OnDisconnected() override;
	void OnSent(JNetwork::ISendPacket* sentPacket, Int32UL sentBytes) override;
	void OnReceived(JNetwork::ICommand* cmd) override;
	void SyncrhonizedOnReceived();
private:
	// JCore::CriticalSectionMutex m_CommandQueueMtx;
	JCore::ArrayQueue<char*> m_CommandQueue;
};

