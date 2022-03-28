/*
	작성자 : 윤정도
*/

#pragma once

#include <JCore/Type.h>

namespace JNetwork {

class TcpClient;
class TcpClientEventListener
{
protected:
	virtual void OnConnected() = 0;
	virtual void OnDisconnected() = 0;
	virtual void OnSent(IPacket* sentPacket, Int32UL sentBytes) = 0;
	virtual void OnReceived(SessionBuffer* buffer, Int32UL receivedBytes) = 0;



	friend class TcpClient;
};

} // namespace JNetwork