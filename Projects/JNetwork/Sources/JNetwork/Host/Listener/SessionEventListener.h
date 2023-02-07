/*
 * 작성자 : 윤정도
 */

#pragma once

#include <JCore/Type.h>
#include <JNetwork/Namespace.h>

NS_JNET_BEGIN

struct ICommand;
struct ISendPacket;

class Session;
class SessionEventListener
{
public:
	SessionEventListener() = default;
	virtual ~SessionEventListener() = default;
protected:
	virtual void OnConnected() = 0;
	virtual void OnDisconnected() = 0;
	virtual void OnSent(ISendPacket* sentPacket, Int32UL sentBytes) = 0;
	virtual void OnReceived(ICommand* cmd) = 0;

	friend class TcpClient;
};

NS_JNET_END