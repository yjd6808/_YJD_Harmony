/*
	작성자 : 윤정도
*/

#pragma once

#include <JCore/Type.h>

namespace JNetwork {

struct ICommand;
struct ISendPacket;
class TcpClient;
class TcpClientEventListener
{
public:
	TcpClientEventListener() = default;
	virtual ~TcpClientEventListener() = default;
protected:
	virtual void OnConnected() = 0;
	virtual void OnDisconnected() = 0;
	virtual void OnSent(ISendPacket* sentPacket, Int32UL sentBytes) = 0;
	virtual void OnReceived(ICommand* cmd) = 0;

	friend class TcpClient;
};

} // namespace JNetwork